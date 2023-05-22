package com.jetbrains.rd.framework.test.cases

import com.jetbrains.rd.framework.*
import com.jetbrains.rd.framework.base.BindState
import com.jetbrains.rd.framework.impl.*
import com.jetbrains.rd.framework.test.util.RdFrameworkTestBase
import com.jetbrains.rd.util.lifetime.Lifetime
import com.jetbrains.rd.util.reactive.IScheduler
import kotlin.test.*

class RdCollectionsTest : RdFrameworkTestBase() {


    override fun createSerializers(isServer: Boolean): ISerializers {
        return super.createSerializers(isServer).apply {
            register(RdMap as ISerializer<RdMap<Int, RdList<RdProperty<RdSet<Int>?>>>>)
            register(RdList as ISerializer<RdList<RdProperty<RdSet<Int>?>>>)
            register(RdSet as ISerializer<RdSet<Int>>)

            register(RdProperty as ISerializer<RdProperty<RdSet<Int>?>>)
            register(RdProperty as ISerializer<RdProperty<RdProperty<RdSet<Int>?>>>)
            register(RdProperty as ISerializer<RdProperty<RdProperty<RdProperty<RdSet<Int>?>>>>)

            register(AsyncRdProperty as ISerializer<AsyncRdProperty<Int>>)
        }
    }

    inline fun <reified T : Any> ISerializers.register(serializer: ISerializer<T>) {
        this.register(object : UniversalMarshaller<T>(
            T::class,
            reader = { ctx, stream -> serializer.read(ctx, stream) },
            writer = { ctx, stream, value -> serializer.write(ctx, stream, value) }
        ) {
            override val id: RdId
                get() = RdId(_type.qualifiedName.getPlatformIndependentHash())
        })
    }

    @Test
    fun setInitializedCollectionsTest() {
        val serverTopLevelProperty = RdProperty<RdMap<Int, RdList<RdProperty<RdSet<Int>?>>>?>(null)
        val clientTopLevelProperty = RdProperty<RdMap<Int, RdList<RdProperty<RdSet<Int>?>>>?>(null)


        serverProtocol.bindStatic(serverTopLevelProperty, 1)
        clientProtocol.bindStatic(clientTopLevelProperty, 1)

        val clientMap = RdMap<Int, RdList<RdProperty<RdSet<Int>?>>>()
        val clientList = RdList<RdProperty<RdSet<Int>?>>()
        val clientProperty = RdProperty<RdSet<Int>?>(null)
        val clientSet = RdSet<Int>()

        clientSet.add(1)
        clientSet.add(2)
        clientSet.add(3)

        clientProperty.value = clientSet
        clientList.add(clientProperty)
        clientMap[2] = clientList

        setSchedulerActive(SchedulerKind.Client) {
            clientTopLevelProperty.value = clientMap
        }

        assertNull(serverTopLevelProperty.value)

        pumpAllProtocols(true)

        val serverMap = serverTopLevelProperty.value
        assertNotNull(serverMap)

        assertEquals(1, serverMap.size)
        assertEquals(2, serverMap.keys.single())

        val serverList = serverMap.values.single()
        assertEquals(1, serverList.size)

        val serverProperty = serverList[0]
        val serverSet = serverProperty.value
        assertNotNull(serverSet)

        assertEquals(3, serverSet.size)
        assertTrue(serverSet.contains(1))
        assertTrue(serverSet.contains(2))
        assertTrue(serverSet.contains(3))

        setSchedulerActive(SchedulerKind.Client) {
            clientTopLevelProperty.value = null
        }

        pumpAllProtocols(true)

        assertEquals(BindState.NotBound, clientMap.bindState)
        assertEquals(BindState.NotBound, serverMap.bindState)

        assertEquals(BindState.NotBound, clientList.bindState)
        assertEquals(BindState.NotBound, serverList.bindState)

        assertEquals(BindState.NotBound, clientProperty.bindState)
        assertEquals(BindState.NotBound, serverProperty.bindState)

        assertEquals(BindState.NotBound, clientSet.bindState)
        assertEquals(BindState.NotBound, serverSet.bindState)
    }

    @Test
    fun unbindTest() {
        val serverTopLevelProperty = RdProperty<RdMap<Int, RdList<RdProperty<RdSet<Int>?>>>?>(null)
        val clientTopLevelProperty = RdProperty<RdMap<Int, RdList<RdProperty<RdSet<Int>?>>>?>(null)


        serverProtocol.bindStatic(serverTopLevelProperty, 1)
        clientProtocol.bindStatic(clientTopLevelProperty, 1)

        val clientMap = RdMap<Int, RdList<RdProperty<RdSet<Int>?>>>()
        var clientList = RdList<RdProperty<RdSet<Int>?>>()
        var clientProperty = RdProperty<RdSet<Int>?>(null)
        var clientSet = RdSet<Int>()

        clientSet.add(1)
        clientSet.add(2)
        clientSet.add(3)

        clientProperty.value = clientSet
        clientList.add(clientProperty)
        clientMap[2] = clientList

        setSchedulerActive(SchedulerKind.Client) {
            clientTopLevelProperty.value = clientMap
        }

        assertNull(serverTopLevelProperty.value)

        pumpAllProtocols(true)

        val serverMap = serverTopLevelProperty.value
        assertNotNull(serverMap)

        assertEquals(1, serverMap.size)
        assertEquals(2, serverMap.keys.single())

        var serverList = serverMap.values.single()
        assertEquals(1, serverList.size)

        var serverProperty = serverList[0]
        var serverSet = serverProperty.value
        assertNotNull(serverSet)

        assertEquals(3, serverSet.size)
        assertTrue(serverSet.contains(1))
        assertTrue(serverSet.contains(2))
        assertTrue(serverSet.contains(3))

        setSchedulerActive(SchedulerKind.Client) {
            clientProperty.value = RdSet<Int>()
        }

        pumpAllProtocols(true)

        assertEquals(BindState.NotBound, clientSet.bindState)
        assertEquals(BindState.NotBound, serverSet.bindState)
        assertEquals(BindState.Bound, clientProperty.bindState)
        assertEquals(BindState.Bound, serverProperty.bindState)

        clientSet = clientProperty.value!!
        serverSet = serverProperty.value!!
        assertEquals(BindState.Bound, clientSet.bindState)
        assertEquals(BindState.Bound, serverSet.bindState)

        setSchedulerActive(SchedulerKind.Client) {
            clientProperty.value = null
        }

        pumpAllProtocols(true)

        assertEquals(BindState.NotBound, clientSet.bindState)
        assertEquals(BindState.NotBound, serverSet.bindState)
        assertEquals(BindState.Bound, clientProperty.bindState)
        assertEquals(BindState.Bound, serverProperty.bindState)

        setSchedulerActive(SchedulerKind.Client) {
            clientList[0] = RdProperty(null)
        }

        pumpAllProtocols(true)

        assertEquals(BindState.NotBound, clientProperty.bindState)
        assertEquals(BindState.NotBound, serverProperty.bindState)
        assertEquals(BindState.Bound, clientList.bindState)
        assertEquals(BindState.Bound, serverList.bindState)


        clientProperty = clientList[0]
        serverProperty = serverList[0]
        assertEquals(BindState.Bound, clientProperty.bindState)
        assertEquals(BindState.Bound, serverProperty.bindState)

        setSchedulerActive(SchedulerKind.Client) {
            clientList.clear()
        }

        pumpAllProtocols(true)

        assertEquals(BindState.NotBound, clientProperty.bindState)
        assertEquals(BindState.NotBound, serverProperty.bindState)
        assertEquals(BindState.Bound, clientList.bindState)
        assertEquals(BindState.Bound, serverList.bindState)

        setSchedulerActive(SchedulerKind.Client) {
            clientMap[clientMap.keys.single()] = RdList()
        }

        pumpAllProtocols(true)

        assertEquals(BindState.NotBound, clientList.bindState)
        assertEquals(BindState.NotBound, serverList.bindState)
        assertEquals(BindState.Bound, clientMap.bindState)
        assertEquals(BindState.Bound, serverMap.bindState)

        clientList = clientMap.values.single()
        serverList = serverMap.values.single()
        assertEquals(BindState.Bound, clientList.bindState)
        assertEquals(BindState.Bound, serverList.bindState)

        setSchedulerActive(SchedulerKind.Client) {
            clientMap.clear()
        }

        pumpAllProtocols(true)

        assertEquals(BindState.NotBound, clientList.bindState)
        assertEquals(BindState.NotBound, serverList.bindState)
        assertEquals(BindState.Bound, clientMap.bindState)
        assertEquals(BindState.Bound, serverMap.bindState)

        setSchedulerActive(SchedulerKind.Client) {
            clientTopLevelProperty.value = null
        }

        pumpAllProtocols(true)

        assertEquals(BindState.NotBound, clientMap.bindState)
        assertEquals(BindState.NotBound, serverMap.bindState)
    }

    @Test
    fun changeCollectionsTest() {
        serverWire.autoFlush = true
        clientWire.autoFlush = true

        val serverTopLevelProperty = RdProperty<RdMap<Int, RdList<RdProperty<RdSet<Int>?>>>?>(null)
        val clientTopLevelProperty = RdProperty<RdMap<Int, RdList<RdProperty<RdSet<Int>?>>>?>(null)

        serverProtocol.bindStatic(serverTopLevelProperty, 1)
        clientProtocol.bindStatic(clientTopLevelProperty, 1)

        val clientMap = RdMap<Int, RdList<RdProperty<RdSet<Int>?>>>()
        val clientList = RdList<RdProperty<RdSet<Int>?>>()
        val clientProperty = RdProperty<RdSet<Int>?>(null)
        val clientSet = RdSet<Int>()
        setSchedulerActive(SchedulerKind.Client) {
            clientTopLevelProperty.value = clientMap

            clientMap[2] = clientList

            clientList.add(clientProperty)

            clientProperty.value = clientSet

            clientSet.add(1)
            clientSet.add(2)
            clientSet.add(3)
        }

        pumpAllProtocols(true)

        var serverMap = serverTopLevelProperty.value
        assertNotNull(serverMap)

        assertEquals(1, serverMap.size)
        assertEquals(2, serverMap.keys.single())

        var serverList = serverMap.values.single()
        assertEquals(1, serverList.size)
        var serverProperty = serverList[0]
        var serverSet = serverProperty.value
        assertNotNull(serverSet)

        assertEquals(3, serverSet.size)
        assertTrue(serverSet.contains(1))
        assertTrue(serverSet.contains(2))
        assertTrue(serverSet.contains(3))
        setSchedulerActive(SchedulerKind.Client) {
            clientTopLevelProperty.value = null
        }

        pumpAllProtocols(true)

        assertEquals(BindState.NotBound,clientMap.bindState)
        assertEquals(BindState.NotBound,serverMap.bindState)

        assertEquals(BindState.NotBound,clientList.bindState)
        assertEquals(BindState.NotBound,serverList.bindState)

        assertEquals(BindState.NotBound,clientProperty.bindState)
        assertEquals(BindState.NotBound,serverProperty.bindState)

        assertEquals(BindState.NotBound,clientSet.bindState)
        assertEquals(BindState.NotBound,serverSet.bindState)
    }

    @Test
    fun propertyTest() {
        serverWire.autoFlush = true
        clientWire.autoFlush = true

        val serverTopLevelProperty = RdProperty<RdProperty<RdProperty<RdSet<Int>?>?>?>(null)
        val clientTopLevelProperty = RdProperty<RdProperty<RdProperty<RdSet<Int>?>?>?>(null)

        serverProtocol.bindStatic(serverTopLevelProperty, 1)
        clientProtocol.bindStatic(clientTopLevelProperty, 1)

        val clientNested1 = RdProperty<RdProperty<RdSet<Int>?>?>(null)
        val clientNested2 = RdProperty<RdSet<Int>?>(null)
        var clientSet = RdSet<Int>()
        clientNested2.value = clientSet
        clientNested1.value = clientNested2

        var serverProperty: RdProperty<RdProperty<RdSet<Int>?>?>?  = null

        setSchedulerActive(SchedulerKind.Server) {
            serverTopLevelProperty.view(serverLifetime) { mapLifetime, property ->
                serverProperty = property
            }
        }

        setSchedulerActive(SchedulerKind.Client) {
            clientTopLevelProperty.value = clientNested1
        }

        pumpAllProtocols(false)

        assertNotNull(serverProperty)
    }

    @Test
    fun asyncPropertyTest() {

        val serverTopLevelProperty = RdProperty<AsyncRdProperty<Int>?>(null)
        val clientTopLevelProperty = RdProperty<AsyncRdProperty<Int>?>(null)

        serverProtocol.bindStatic(serverTopLevelProperty, 1)
        clientProtocol.bindStatic(clientTopLevelProperty, 1)

        val clientAsyncProperty = AsyncRdProperty<Int>()

        var serverAsyncProperty: AsyncRdProperty<Int>?  = null

        setSchedulerActive(SchedulerKind.Server) {
            serverTopLevelProperty.view(serverLifetime) { mapLifetime, property ->
                serverAsyncProperty = property
            }
        }

        setSchedulerActive(SchedulerKind.Client) {
            clientTopLevelProperty.value = clientAsyncProperty
        }

        pumpAllProtocols(true)

        assertNotNull(serverAsyncProperty)
        assertFalse(serverAsyncProperty!!.maybe.hasValue)

        clientAsyncProperty.value = 123
        assertTrue(serverAsyncProperty!!.maybe.hasValue)
        assertEquals(123, serverAsyncProperty!!.value)
    }



    enum class SchedulerKind {
        Client,
        Server,
        Both
    }

    private fun <T> setSchedulerActive(kind: SchedulerKind, action: () -> T): T {
        return Lifetime.using { lifetime ->
            if (kind == SchedulerKind.Client || kind == SchedulerKind.Both)
                (clientProtocol.scheduler as PumpScheduler).setActive(lifetime)

            if (kind == SchedulerKind.Server || kind == SchedulerKind.Both)
                (serverProtocol.scheduler as PumpScheduler).setActive(lifetime)

            action()
        }

    }

    override val clientScheduler: IScheduler
        get() = PumpScheduler()
    override val serverScheduler: IScheduler
        get() = PumpScheduler()

    override val clientWireScheduler: IScheduler?
        get() = MyScheduler()
    override val serverWireScheduler: IScheduler?
        get() = MyScheduler()

    private fun pumpAllProtocols(transmitMessages: Boolean) {
        val clientScheduler = clientProtocol.scheduler as PumpScheduler
        val serverScheduler = serverProtocol.scheduler as PumpScheduler
        do {
            if (transmitMessages) {
                if (clientWire.hasMessages) clientWire.processAllMessages()
                if (serverWire.hasMessages) serverWire.processAllMessages()
            }
        } while (clientScheduler.pumpOnce() || serverScheduler.pumpOnce() || clientWire.hasMessages || serverWire.hasMessages)
    }

    override fun tearDown() {
        (clientProtocol.scheduler as PumpScheduler).close()
        (serverProtocol.scheduler as PumpScheduler).close()

        super.tearDown()
    }

    private class MyScheduler : IScheduler {
        private val thread: Thread = Thread.currentThread()
        private var count = 0

        override fun queue(action: () -> Unit) {
            assertThread()

            count++
            try {
                action()
            } finally {
                count--
            }
        }

        private fun assertThread() {
            assertEquals(thread, Thread.currentThread(), "Wrong thread. Expected: $thread, Actual: ${Thread.currentThread()}")
        }

        override val isActive: Boolean
            get()  {
                assertThread()
                return count > 0
            }

        override fun flush() {
            error("Not supported")
        }
    }

    private class PumpScheduler : IScheduler, AutoCloseable {
        private val thread: Thread = Thread.currentThread()
        private var count = 0
        private var disposed = false
        private val actions = ArrayDeque<() -> Unit>()

        override fun queue(action: () -> Unit) {
            assertThread()

            if (disposed) {
                action()
                return
            }

            actions.add {
                count++
                try {
                    action()
                } finally {
                    count--
                }
            }
        }

        fun setActive(lifetime: Lifetime) {
            assertThread()
            lifetime.bracketOrThrow({
                count++
            }, {
                assertThread()
                count--
            })
        }

        override val isActive: Boolean
            get()  {
                assertThread()
                return count > 0
            }

        override fun flush() {
            error("Not supported")
        }

        private fun assertThread() {
            assertEquals(thread, Thread.currentThread(), "Wrong thread. Expected: $thread, Actual: ${Thread.currentThread()}")
        }

        fun pumpOnce() :Boolean {
            assertThread()
            val action = actions.removeFirstOrNull() ?: return false
            action()
            return true
        }

        fun pumpAll() {
            while (pumpOnce()) {

            }
        }

        override fun close() {
            count = 1
            disposed = true
        }

    }
}