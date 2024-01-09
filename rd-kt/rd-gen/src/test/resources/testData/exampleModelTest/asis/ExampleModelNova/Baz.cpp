#include "Baz.Generated.h"



#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable:4250 )
#pragma warning( disable:4307 )
#pragma warning( disable:4267 )
#pragma warning( disable:4244 )
#pragma warning( disable:4100 )
#endif

namespace org.example {
// companion
// constants
constexpr std::wstring Baz::const_nls;
constexpr std::wstring Baz::const_for_default_nls;
// initializer
void Baz::initialize()
{
    mapScalar_.optimize_nested = true;
    property_with_default_nls_.optimize_nested = true;
    property_with_several_attrs_.optimize_nested = true;
    nls_prop_.optimize_nested = true;
    nullable_nls_prop_.optimize_nested = true;
    duration_prop_.optimize_nested = true;
    mapScalar_.async = true;
}
// primary ctor
Baz::Baz(std::vector<rd::Wrapper<Foo>> foo_, std::vector<rd::Wrapper<A>> bar_, rd::Wrapper<std::wstring> nls_field_, rd::Wrapper<std::wstring> nls_nullable_field_, std::vector<rd::Wrapper<std::wstring>> string_list_field_, std::vector<rd::Wrapper<std::wstring>> nls_list_field_, rd::RdProperty<rd::Wrapper<Foo>, Baz::__FooNullableSerializer> foo1_, rd::RdProperty<rd::Wrapper<A>, Baz::__ANullableSerializer> bar1_, rd::RdMap<int32_t, ScalarPrimer, rd::Polymorphic<int32_t>, rd::AbstractPolymorphic<ScalarPrimer>> mapScalar_, rd::RdMap<int32_t, FooBar, rd::Polymorphic<int32_t>, rd::Polymorphic<FooBar>> mapBindable_, rd::RdProperty<std::wstring, Baz::__StringSerializer> property_with_default_nls_, rd::RdProperty<std::wstring, Baz::__StringSerializer> property_with_several_attrs_, rd::RdProperty<std::wstring, Baz::__StringSerializer> nls_prop_, rd::RdProperty<rd::Wrapper<std::wstring>, Baz::__StringNullableSerializer> nullable_nls_prop_, rd::Wrapper<std::wstring> non_nls_open_field_, rd::RdProperty<rd::TimeSpan, rd::Polymorphic<rd::TimeSpan>> duration_prop_, rd::RdProperty<std::wstring, rd::Polymorphic<std::wstring>> y_, rd::RdProperty<Z, rd::Polymorphic<Z>> z_, int32_t x_, rd::RdMap<int32_t, int32_t, rd::Polymorphic<int32_t>, rd::Polymorphic<int32_t>> sdf_) :
A(std::move(y_),std::move(z_),std::move(x_),std::move(sdf_))
,foo_(std::move(foo_)), bar_(std::move(bar_)), nls_field_(std::move(nls_field_)), nls_nullable_field_(std::move(nls_nullable_field_)), string_list_field_(std::move(string_list_field_)), nls_list_field_(std::move(nls_list_field_)), foo1_(std::move(foo1_)), bar1_(std::move(bar1_)), mapScalar_(std::move(mapScalar_)), mapBindable_(std::move(mapBindable_)), property_with_default_nls_(std::move(property_with_default_nls_)), property_with_several_attrs_(std::move(property_with_several_attrs_)), nls_prop_(std::move(nls_prop_)), nullable_nls_prop_(std::move(nullable_nls_prop_)), non_nls_open_field_(std::move(non_nls_open_field_)), duration_prop_(std::move(duration_prop_))
{
    initialize();
}
// secondary constructor
Baz::Baz(std::vector<rd::Wrapper<Foo>> foo_, std::vector<rd::Wrapper<A>> bar_, rd::Wrapper<std::wstring> nls_field_, rd::Wrapper<std::wstring> nls_nullable_field_, std::vector<rd::Wrapper<std::wstring>> string_list_field_, std::vector<rd::Wrapper<std::wstring>> nls_list_field_, rd::Wrapper<std::wstring> non_nls_open_field_, int32_t x_) : 
Baz((std::move(foo_)),(std::move(bar_)),(std::move(nls_field_)),(std::move(nls_nullable_field_)),(std::move(string_list_field_)),(std::move(nls_list_field_)),{},{},{},{},{},{},{},{},(std::move(non_nls_open_field_)),{},{},{},(std::move(x_)),{})
{
    initialize();
}
// default ctors and dtors
// reader
Baz Baz::read(rd::SerializationCtx& ctx, rd::Buffer & buffer)
{
    auto _id = rd::RdId::read(buffer);
    auto y_ = rd::RdProperty<std::wstring, rd::Polymorphic<std::wstring>>::read(ctx, buffer);
    auto z_ = rd::RdProperty<Z, rd::Polymorphic<Z>>::read(ctx, buffer);
    auto x_ = buffer.read_integral<int32_t>();
    auto sdf_ = rd::RdMap<int32_t, int32_t, rd::Polymorphic<int32_t>, rd::Polymorphic<int32_t>>::read(ctx, buffer);
    auto foo_ = buffer.read_array<std::vector, Foo, rd::allocator<rd::Wrapper<Foo>>>(
    [&ctx, &buffer]() mutable
    { return ctx.get_serializers().readPolymorphic<Foo>(ctx, buffer); }
    );
    auto bar_ = buffer.read_array<std::vector, rd::Wrapper<A>, rd::allocator<rd::Wrapper<A>>>(
    [&ctx, &buffer]() mutable
    { return buffer.read_nullable<A>(
    [&ctx, &buffer]() mutable
    { return ctx.get_serializers().readPolymorphic<A>(ctx, buffer); }
    ); }
    );
    auto nls_field_ = buffer.read_wstring();
    auto nls_nullable_field_ = buffer.read_nullable<std::wstring>(
    [&ctx, &buffer]() mutable
    { return buffer.read_wstring(); }
    );
    auto string_list_field_ = buffer.read_array<std::vector, std::wstring, rd::allocator<rd::Wrapper<std::wstring>>>(
    [&ctx, &buffer]() mutable
    { return buffer.read_wstring(); }
    );
    auto nls_list_field_ = buffer.read_array<std::vector, std::wstring, rd::allocator<rd::Wrapper<std::wstring>>>(
    [&ctx, &buffer]() mutable
    { return buffer.read_wstring(); }
    );
    auto foo1_ = rd::RdProperty<rd::Wrapper<Foo>, Baz::__FooNullableSerializer>::read(ctx, buffer);
    auto bar1_ = rd::RdProperty<rd::Wrapper<A>, Baz::__ANullableSerializer>::read(ctx, buffer);
    auto mapScalar_ = rd::RdMap<int32_t, ScalarPrimer, rd::Polymorphic<int32_t>, rd::AbstractPolymorphic<ScalarPrimer>>::read(ctx, buffer);
    auto mapBindable_ = rd::RdMap<int32_t, FooBar, rd::Polymorphic<int32_t>, rd::Polymorphic<FooBar>>::read(ctx, buffer);
    auto property_with_default_nls_ = rd::RdProperty<std::wstring, Baz::__StringSerializer>::read(ctx, buffer);
    auto property_with_several_attrs_ = rd::RdProperty<std::wstring, Baz::__StringSerializer>::read(ctx, buffer);
    auto nls_prop_ = rd::RdProperty<std::wstring, Baz::__StringSerializer>::read(ctx, buffer);
    auto nullable_nls_prop_ = rd::RdProperty<rd::Wrapper<std::wstring>, Baz::__StringNullableSerializer>::read(ctx, buffer);
    auto non_nls_open_field_ = buffer.read_wstring();
    auto duration_prop_ = rd::RdProperty<rd::TimeSpan, rd::Polymorphic<rd::TimeSpan>>::read(ctx, buffer);
    Baz res{std::move(foo_), std::move(bar_), std::move(nls_field_), std::move(nls_nullable_field_), std::move(string_list_field_), std::move(nls_list_field_), std::move(foo1_), std::move(bar1_), std::move(mapScalar_), std::move(mapBindable_), std::move(property_with_default_nls_), std::move(property_with_several_attrs_), std::move(nls_prop_), std::move(nullable_nls_prop_), std::move(non_nls_open_field_), std::move(duration_prop_), std::move(y_), std::move(z_), std::move(x_), std::move(sdf_)};
    withId(res, _id);
    return res;
}
// writer
void Baz::write(rd::SerializationCtx& ctx, rd::Buffer& buffer) const
{
    this->rdid.write(buffer);
    y_.write(ctx, buffer);
    z_.write(ctx, buffer);
    buffer.write_integral(x_);
    sdf_.write(ctx, buffer);
    buffer.write_array<std::vector, Foo, rd::allocator<rd::Wrapper<Foo>>>(foo_,
    [&ctx, &buffer](Foo const & it) mutable  -> void
    { ctx.get_serializers().writePolymorphic<Foo>(ctx, buffer, it); }
    );
    buffer.write_array<std::vector, rd::Wrapper<A>, rd::allocator<rd::Wrapper<A>>>(bar_,
    [&ctx, &buffer](rd::Wrapper<A> const & it) mutable  -> void
    { buffer.write_nullable<A>(it,
    [&ctx, &buffer](rd::Wrapper<A> const & it) mutable  -> void
    { ctx.get_serializers().writePolymorphic<A>(ctx, buffer, it); }
    ); }
    );
    buffer.write_wstring(nls_field_);
    buffer.write_nullable<std::wstring>(nls_nullable_field_,
    [&ctx, &buffer](rd::Wrapper<std::wstring> const & it) mutable  -> void
    { buffer.write_wstring(it); }
    );
    buffer.write_array<std::vector, std::wstring, rd::allocator<rd::Wrapper<std::wstring>>>(string_list_field_,
    [&ctx, &buffer](std::wstring const & it) mutable  -> void
    { buffer.write_wstring(it); }
    );
    buffer.write_array<std::vector, std::wstring, rd::allocator<rd::Wrapper<std::wstring>>>(nls_list_field_,
    [&ctx, &buffer](std::wstring const & it) mutable  -> void
    { buffer.write_wstring(it); }
    );
    foo1_.write(ctx, buffer);
    bar1_.write(ctx, buffer);
    mapScalar_.write(ctx, buffer);
    mapBindable_.write(ctx, buffer);
    property_with_default_nls_.write(ctx, buffer);
    property_with_several_attrs_.write(ctx, buffer);
    nls_prop_.write(ctx, buffer);
    nullable_nls_prop_.write(ctx, buffer);
    buffer.write_wstring(non_nls_open_field_);
    duration_prop_.write(ctx, buffer);
}
// virtual init
void Baz::init(rd::Lifetime lifetime) const
{
    rd::RdBindableBase::init(lifetime);
    bindPolymorphic(foo_, lifetime, this, "foo");
    bindPolymorphic(bar_, lifetime, this, "bar");
    bindPolymorphic(foo1_, lifetime, this, "foo1");
    bindPolymorphic(bar1_, lifetime, this, "bar1");
    bindPolymorphic(mapScalar_, lifetime, this, "mapScalar");
    bindPolymorphic(mapBindable_, lifetime, this, "mapBindable");
    bindPolymorphic(property_with_default_nls_, lifetime, this, "property_with_default_nls");
    bindPolymorphic(property_with_several_attrs_, lifetime, this, "property_with_several_attrs");
    bindPolymorphic(nls_prop_, lifetime, this, "nls_prop");
    bindPolymorphic(nullable_nls_prop_, lifetime, this, "nullable_nls_prop");
    bindPolymorphic(duration_prop_, lifetime, this, "duration_prop");
}
// identify
void Baz::identify(const rd::Identities &identities, rd::RdId const &id) const
{
    rd::RdBindableBase::identify(identities, id);
    identifyPolymorphic(foo_, identities, id.mix(".foo"));
    identifyPolymorphic(bar_, identities, id.mix(".bar"));
    identifyPolymorphic(foo1_, identities, id.mix(".foo1"));
    identifyPolymorphic(bar1_, identities, id.mix(".bar1"));
    identifyPolymorphic(mapScalar_, identities, id.mix(".mapScalar"));
    identifyPolymorphic(mapBindable_, identities, id.mix(".mapBindable"));
    identifyPolymorphic(property_with_default_nls_, identities, id.mix(".property_with_default_nls"));
    identifyPolymorphic(property_with_several_attrs_, identities, id.mix(".property_with_several_attrs"));
    identifyPolymorphic(nls_prop_, identities, id.mix(".nls_prop"));
    identifyPolymorphic(nullable_nls_prop_, identities, id.mix(".nullable_nls_prop"));
    identifyPolymorphic(duration_prop_, identities, id.mix(".duration_prop"));
}
// getters
std::vector<rd::Wrapper<Foo>> const & Baz::get_foo() const
{
    return foo_;
}
std::vector<rd::Wrapper<A>> const & Baz::get_bar() const
{
    return bar_;
}
std::wstring const & Baz::get_nls_field() const
{
    return nls_field_;
}
rd::Wrapper<std::wstring> const & Baz::get_nls_nullable_field() const
{
    return nls_nullable_field_;
}
std::vector<rd::Wrapper<std::wstring>> const & Baz::get_string_list_field() const
{
    return string_list_field_;
}
std::vector<rd::Wrapper<std::wstring>> const & Baz::get_nls_list_field() const
{
    return nls_list_field_;
}
rd::IProperty<rd::Wrapper<Foo>> const & Baz::get_foo1() const
{
    return foo1_;
}
rd::IProperty<rd::Wrapper<A>> const & Baz::get_bar1() const
{
    return bar1_;
}
rd::IViewableMap<int32_t, ScalarPrimer> const & Baz::get_mapScalar() const
{
    return mapScalar_;
}
rd::IViewableMap<int32_t, FooBar> const & Baz::get_mapBindable() const
{
    return mapBindable_;
}
rd::IProperty<std::wstring> const & Baz::get_property_with_default_nls() const
{
    return property_with_default_nls_;
}
rd::IProperty<std::wstring> const & Baz::get_property_with_several_attrs() const
{
    return property_with_several_attrs_;
}
rd::IProperty<std::wstring> const & Baz::get_nls_prop() const
{
    return nls_prop_;
}
rd::IProperty<rd::Wrapper<std::wstring>> const & Baz::get_nullable_nls_prop() const
{
    return nullable_nls_prop_;
}
std::wstring const & Baz::get_non_nls_open_field() const
{
    return non_nls_open_field_;
}
rd::IProperty<rd::TimeSpan> const & Baz::get_duration_prop() const
{
    return duration_prop_;
}
// intern
// equals trait
bool Baz::equals(rd::ISerializable const& object) const
{
    auto const &other = dynamic_cast<Baz const&>(object);
    return this == &other;
}
// equality operators
bool operator==(const Baz &lhs, const Baz &rhs) {
    return &lhs == &rhs;
}
bool operator!=(const Baz &lhs, const Baz &rhs){
    return !(lhs == rhs);
}
// hash code trait
// type name trait
std::string Baz::type_name() const
{
    return "Baz";
}
// static type name trait
std::string Baz::static_type_name()
{
    return "Baz";
}
// polymorphic to string
std::string Baz::toString() const
{
    std::string res = "Baz\n";
    res += "\tfoo = ";
    res += rd::to_string(foo_);
    res += '\n';
    res += "\tbar = ";
    res += rd::to_string(bar_);
    res += '\n';
    res += "\tnls_field = ";
    res += rd::to_string(nls_field_);
    res += '\n';
    res += "\tnls_nullable_field = ";
    res += rd::to_string(nls_nullable_field_);
    res += '\n';
    res += "\tstring_list_field = ";
    res += rd::to_string(string_list_field_);
    res += '\n';
    res += "\tnls_list_field = ";
    res += rd::to_string(nls_list_field_);
    res += '\n';
    res += "\tfoo1 = ";
    res += rd::to_string(foo1_);
    res += '\n';
    res += "\tbar1 = ";
    res += rd::to_string(bar1_);
    res += '\n';
    res += "\tmapScalar = ";
    res += rd::to_string(mapScalar_);
    res += '\n';
    res += "\tmapBindable = ";
    res += rd::to_string(mapBindable_);
    res += '\n';
    res += "\tproperty_with_default_nls = ";
    res += rd::to_string(property_with_default_nls_);
    res += '\n';
    res += "\tproperty_with_several_attrs = ";
    res += rd::to_string(property_with_several_attrs_);
    res += '\n';
    res += "\tnls_prop = ";
    res += rd::to_string(nls_prop_);
    res += '\n';
    res += "\tnullable_nls_prop = ";
    res += rd::to_string(nullable_nls_prop_);
    res += '\n';
    res += "\tnon_nls_open_field = ";
    res += rd::to_string(non_nls_open_field_);
    res += '\n';
    res += "\tduration_prop = ";
    res += rd::to_string(duration_prop_);
    res += '\n';
    res += "\ty = ";
    res += rd::to_string(y_);
    res += '\n';
    res += "\tz = ";
    res += rd::to_string(z_);
    res += '\n';
    res += "\tx = ";
    res += rd::to_string(x_);
    res += '\n';
    res += "\tsdf = ";
    res += rd::to_string(sdf_);
    res += '\n';
    return res;
}
// external to string
std::string to_string(const Baz & value)
{
    return value.toString();
}
}

#ifdef _MSC_VER
#pragma warning( pop )
#endif

