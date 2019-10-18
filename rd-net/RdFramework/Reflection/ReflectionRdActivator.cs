﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using JetBrains.Annotations;
using JetBrains.Collections.Viewable;
using JetBrains.Core;
using JetBrains.Diagnostics;
using JetBrains.Lifetimes;
using JetBrains.Rd.Base;
using JetBrains.Rd.Impl;
using JetBrains.Rd.Tasks;
using JetBrains.Util;
#if NET35
using TypeInfo = System.Type;
#else
using System.Threading.Tasks;
#endif

namespace JetBrains.Rd.Reflection
{
  /// <summary>
	/// To get maximal performance of  Rd framework you should directly provide serializers to every Model and Property.
	///
	/// Creating models by hand with this approach is tedious and error-prone.  The main idea behind <see
	/// cref="ReflectionRdActivator" /> is to automatically create RdExt and initialize fields and properties with
	/// appropriate serializers.
	/// 
  /// </summary>
  public class ReflectionRdActivator
  {
    [NotNull] private readonly ReflectionSerializersFactory mySerializersFactory;
    [NotNull] private readonly ProxyGenerator myProxyGenerator;
    [CanBeNull] private readonly IPolymorphicTypesCatalog myPolymorphicTypesCatalog;

    [NotNull]
    public ReflectionSerializersFactory SerializersFactory => mySerializersFactory;

    [NotNull]
    public ProxyGenerator Generator => myProxyGenerator;

    [CanBeNull]
    public IPolymorphicTypesCatalog TypesCatalog => myPolymorphicTypesCatalog;

#if JET_MODE_ASSERT
    /// <summary>
    /// current activation stack.
    ///
    /// used to protect from circular dependencies only.
    /// </summary>
    [ThreadStatic]
    private static Queue<Type> myCurrentActivationChain;

#endif

    public ReflectionRdActivator([NotNull] ReflectionSerializersFactory serializersFactory, [CanBeNull] IPolymorphicTypesCatalog polymorphicTypesCatalog)
      : this(serializersFactory, new ProxyGenerator(), polymorphicTypesCatalog)
    {
    }

    public ReflectionRdActivator([NotNull] ReflectionSerializersFactory serializersFactory, [NotNull] ProxyGenerator proxyGenerator, [CanBeNull] IPolymorphicTypesCatalog polymorphicTypesCatalog)
    {
      mySerializersFactory = serializersFactory;
      myPolymorphicTypesCatalog = polymorphicTypesCatalog;
      myProxyGenerator = proxyGenerator;
    }

    /// <summary>
    /// Create and bind class with <see cref="RdExtAttribute"/>
    /// </summary>
    /// <typeparam name="T"></typeparam>
    /// <returns></returns>
    [NotNull]
    public T ActivateBind<T>(Lifetime lifetime, [NotNull] IProtocol protocol) where T : RdBindableBase
    {
      var instance = ActivateRdExt<T>();

      var typename = typeof(T).Name;
      instance.Identify(protocol.Identities, RdId.Root.Mix(typename));
      instance.Bind(lifetime, protocol, typename);

      return instance;
    }

    /// <summary>
    /// Create and bind class with <see cref="RdExtAttribute"/>
    /// </summary>
    /// <returns></returns>
    [NotNull, PublicAPI]
    public RdBindableBase ActivateBind(Type type, Lifetime lifetime, [NotNull] IProtocol protocol)
    {
      var instance = ActivateRdExt(type);

      var typename = type.Name;
      var bindable = (RdBindableBase) instance;
      bindable.Identify(protocol.Identities, RdId.Root.Mix(typename));
      bindable.Bind(lifetime, protocol, typename);

      return bindable;
    }


    /// <summary>
    /// Create and initialize RdModel root and its members (including nested RdModels)
    ///
    /// It doesn't bind model to Protocol. You may want to use <see cref="ActivateBind{T}"/>
    /// </summary>
    public T ActivateRdExt<T>() where T : RdBindableBase
    {
      return (T) ActivateRdExt(typeof(T));
    }

    /// <summary>
    /// Activate <see cref="RdExtAttribute"/> and its members.
    /// Can't be used for data models
    /// </summary>
    /// <param name="type"></param>
    /// <returns></returns>
    [NotNull]
    public object ActivateRdExt(Type type)
    {
      #if JET_MODE_ASSERT
      myCurrentActivationChain = myCurrentActivationChain ?? new Queue<Type>();
      myCurrentActivationChain.Clear(); // clear previous attempts to activate different types
      #endif

      // We should register serializer for current type and all of it members to have possibility to get valid serializers for arguments.
      myPolymorphicTypesCatalog?.AddType(type);

      return ActivateRd(type);
    }

    private object ActivateRd(Type type)
    {
#if JET_MODE_ASSERT
      Assertion.Assert(!myCurrentActivationChain.Contains(type),
        $"Unable to activate {type.FullName}: circular dependency detected: {string.Join(" -> ", myCurrentActivationChain.Select(t => t.FullName).ToArray())}");
      myCurrentActivationChain.Enqueue(type);
#endif

      var typeInfo = type.GetTypeInfo();
      ReflectionSerializerVerifier.AssertValidRdExt(typeInfo);
      var implementingType = ReflectionSerializerVerifier.GetImplementingType(typeInfo);
      Assertion.Assert(typeof(IRdBindable).GetTypeInfo().IsAssignableFrom(implementingType),
        $"Unable to activate {type.FullName}: type should be {nameof(IRdBindable)}");

      var instance = Activator.CreateInstance(implementingType);
      var implementingTypeInfo = implementingType.GetTypeInfo();

      foreach (var mi in ReflectionSerializersFactory.GetBindableMembers(implementingTypeInfo))
      {
        ReflectionSerializerVerifier.AssertMemberDeclaration(mi);
        var currentValue = ReflectionUtil.GetGetter(mi)(instance);
        if (currentValue == null)
        {
          currentValue = ActivateRdExtMember(mi);

          var memberSetter = ReflectionUtil.GetSetter(mi);
          memberSetter(instance, currentValue);
        }
      }

      // Add RdEndpoint for Impl class (counterpart of Proxy)
      var interfaces = implementingTypeInfo.GetInterfaces();
      bool isProxy = interfaces.Contains(typeof(IProxyTypeMarker));
      var rpcInterface = ReflectionSerializersFactory.GetRpcInterface(typeInfo);
      if (!isProxy && rpcInterface != null)
      {
        var fieldInfo = implementingTypeInfo.GetField("BindableChildren", BindingFlags.Instance | BindingFlags.NonPublic).NotNull("BindableChildren not found");
        var bindableChildren = (List<KeyValuePair<string, object>>) fieldInfo.GetValue(instance);

        var implementingMethods = typeInfo.GetInterfaceMap(rpcInterface).TargetMethods;
        var adapters = myProxyGenerator.CreateAdapter(implementingType, implementingMethods);

        for (var i = 0; i < implementingMethods.Length; i++)
        {
          var implMethod = implementingMethods[i];
          var name = implMethod.Name + "_proxy";
          var requestType = ProxyGenerator.GetRequstType(implMethod)[0];
          EnsureFakeTupleRegistered(requestType);

          var responseNonTaskType = ProxyGenerator.GetResponseType(implMethod, unwrapTask: true);
          var responseType = ProxyGenerator.GetResponseType(implMethod, unwrapTask: false);
          var endPointType = typeof(RdCall<,>).MakeGenericType(requestType, responseNonTaskType);
          var endpoint = ActivateGenericMember(name, endPointType.GetTypeInfo());
          endPointType.GetProperty(nameof(RdCall<int, int>.ValueCanBeNull)).NotNull().SetValue(endpoint, true, null);
          if (ProxyGenerator.IsSync(implMethod))
          {
            var delType = typeof(Func<,,>).MakeGenericType(typeof(Lifetime), requestType, typeof(RdTask<>).MakeGenericType(responseNonTaskType));
            var @delegate = adapters[i].CreateDelegate(delType, instance);
            var methodInfo = typeof(ReflectionRdActivator).GetMethod(nameof(SetHandler)).NotNull().MakeGenericMethod(requestType, responseNonTaskType);
            methodInfo.Invoke(null, new[] {endpoint, @delegate});
          }
          else
          {
#if NET35
            throw new NotSupportedException("Async methods not supported in NET35");
#else
            if (responseType == typeof(Task))
            {
              var delType = typeof(Func<,,>).MakeGenericType(typeof(Lifetime), requestType, typeof(Task));
              var @delegate = adapters[i].CreateDelegate(delType, instance);
              var methodInfo = typeof(ReflectionRdActivator).GetMethod(nameof(SetHandlerTaskVoid)).NotNull().MakeGenericMethod(requestType);
              methodInfo.Invoke(null, new[] {endpoint, @delegate});
            }
            else
            {
              var delType = typeof(Func<,,>).MakeGenericType(typeof(Lifetime), requestType, typeof(Task<>).MakeGenericType(responseNonTaskType));
              var @delegate = adapters[i].CreateDelegate(delType, instance);
              MethodInfo methodInfo;
              methodInfo = typeof(ReflectionRdActivator).GetMethod(nameof(SetHandlerTask)).NotNull().MakeGenericMethod(requestType, responseNonTaskType);
              methodInfo.Invoke(null, new[] {endpoint, @delegate});
            }
#endif
          }

          bindableChildren.Add(new KeyValuePair<string, object>(name, endpoint));
        }
      }
      else if (rpcInterface != null)
      {
        foreach (var interfaceMethod in rpcInterface.GetMethods())
        {
          var requestType = ProxyGenerator.GetRequstType(interfaceMethod)[0];
          EnsureFakeTupleRegistered(requestType);
        }
      }

#if JET_MODE_ASSERT
      myCurrentActivationChain.Dequeue();
#endif
      // Allow initialize to setup bindings to composite properties.
      if (instance is RdReflectionBindableBase reflectionBindable)
      {
        reflectionBindable.OnActivated();
      }

      return instance;
    }

    private void EnsureFakeTupleRegistered(Type type)
    {
      Assertion.AssertNotNull(myPolymorphicTypesCatalog, "myPolymorphicTypesCatalog required to be NotNull when RPC is used");
      myPolymorphicTypesCatalog.AddType(type);
    }

#if !NET35
    /// <summary>
    ///  Wrapper method to simplify search with overload resolution for two methods in RdEndpoint
    /// </summary>
    public static void SetHandlerTask<TReq, TRes>(RdCall<TReq, TRes> endpoint, Func<Lifetime, TReq, Task<TRes>> handler)
    {
      endpoint.Set(handler);
    }

    /// <summary>
    ///  Wrapper method to simplify search with overload resolution for two methods in RdEndpoint
    /// </summary>
    public static void SetHandlerTaskVoid<TReq>(RdCall<TReq, Unit> endpoint, Func<Lifetime, TReq, Task> handler)
    {
      endpoint.Set((lt, req) => handler(lt, req).ToRdTask());
    }
#endif

    /// <summary>
    ///  Wrapper method to simplify search with overload resolution for two methods in RdEndpoint
    /// </summary>
    public static void SetHandler<TReq, TRes>(RdCall<TReq, TRes> endpoint, Func<Lifetime, TReq, RdTask<TRes>> handler)
    {
      endpoint.Set(handler);
    }
    
    private object ActivateRdExtMember(MemberInfo mi)
    {
      var returnType = ReflectionUtil.GetReturnType(mi);
      var typeInfo = returnType.GetTypeInfo();
      var implementingType = ReflectionSerializerVerifier.GetImplementingType(typeInfo);

      object result;
      if (implementingType.GetTypeInfo().IsGenericType)
      {
        result = ActivateGenericMember(mi.Name, typeInfo);
      }
      else
      {
        result = ActivateRd(returnType);
      }

      if (result is IRdReactive activatedBindable)
      {
        foreach (var _ in mi.GetCustomAttributes(typeof(RdAsyncAttribute), false))
          activatedBindable.Async = true;
      }

      return result;
    }

    private bool CanBePolymorphic(TypeInfo typeInfo)
    {
      return !typeInfo.IsSealed || typeInfo.BaseType != typeof(RdBindableBase);
    }

    private ReflectionSerializersFactory.SerializerPair GetPolymorphic(Type argument)
    {
      var polymorphicClass = typeof(Polymorphic<>).MakeGenericType(argument);
      var reader = polymorphicClass.GetTypeInfo().GetField("Read", BindingFlags.Public | BindingFlags.Static).NotNull().GetValue(argument);
      var writer = polymorphicClass.GetTypeInfo().GetField("Write", BindingFlags.Public | BindingFlags.Static).NotNull().GetValue(argument);
      return new ReflectionSerializersFactory.SerializerPair(reader, writer);
    }

    private ReflectionSerializersFactory.SerializerPair GetProperSerializer(Type type)
    {
      myPolymorphicTypesCatalog?.AddType(type);

      if (CanBePolymorphic(type.GetTypeInfo()))
      {
        return GetPolymorphic(type);
      }
      else
      {
        return mySerializersFactory.GetOrRegisterSerializerPair(type);
      }
    }

    private object ActivateGenericMember(string memberName, TypeInfo memberType)
    {
      var implementingType = ReflectionSerializerVerifier.GetImplementingType(memberType);
      var genericDefinition = implementingType.GetGenericTypeDefinition();

      var genericArguments = implementingType.GetTypeInfo().GetGenericArguments();
      var argument = genericArguments[0];
      var serializerPair = GetProperSerializer(argument);

      if (genericDefinition == typeof(RdProperty<>) ||
          genericDefinition == typeof(RdSignal<>) ||
          genericDefinition == typeof(RdSet<>))
      {
        return Activator.CreateInstance(implementingType, serializerPair.Reader, serializerPair.Writer);
      }

      if (genericDefinition == typeof(RdList<>))
      {
        return Activator.CreateInstance(implementingType, serializerPair.Reader, serializerPair.Writer, 1L /*nextVersion*/);
      }

      if (genericDefinition == typeof(RdMap<,>))
      {
        var argument2 = genericArguments[1];
        var serializerPair2 = GetProperSerializer(argument2);
        return Activator.CreateInstance(implementingType, serializerPair.Reader, serializerPair.Writer, serializerPair2.Reader, serializerPair2.Writer);
      }

      if (genericDefinition == typeof(InprocRpc<,>) || genericDefinition == typeof(RdCall<,>) || genericDefinition == typeof(RdCall<,>))
      {
        var rpcResultType = genericArguments[1];
        GetProperSerializer(rpcResultType);
        var rdCallInstance = Activator.CreateInstance(implementingType);
        implementingType.GetProperty(nameof(RdCall<int, int>.ValueCanBeNull)).NotNull().SetValue(rdCallInstance, true, null);
        return rdCallInstance;
      }

      if (genericDefinition == typeof(Nullable<>))
      {
        // already initialized to null
        return null;
      }

      // hack for UProperty
      if (genericArguments.Length == 1 &&
          typeof(IViewableProperty<>).MakeGenericType(genericArguments).GetTypeInfo().IsAssignableFrom(implementingType))
      {
        foreach (var ctor in implementingType.GetTypeInfo().GetConstructors(BindingFlags.Public | BindingFlags.Instance))
        {
          var parameters = ctor.GetParameters();
          if (parameters.Length == 3 && parameters[0].Name == "id")
          {
            return Activator.CreateInstance(implementingType, memberName, serializerPair.Reader, serializerPair.Writer);
          }
        }
      }

      throw new Exception($"Unable to activate generic type: {memberType}");
    }
  }
}