set(P3DTOOLBASE_HEADER_FILES
    addHash.I addHash.h
    atomicAdjust.h
    atomicAdjustDummyImpl.h atomicAdjustDummyImpl.I
    atomicAdjustI386Impl.h atomicAdjustI386Impl.I
    atomicAdjustPosixImpl.h atomicAdjustPosixImpl.I
    atomicAdjustWin32Impl.h atomicAdjustWin32Impl.I
    cmath.I cmath.h
    deletedBufferChain.h deletedBufferChain.I
    deletedChain.h deletedChain.T
    dtoolbase.h dtoolbase_cc.h dtoolsymbols.h
    dtool_platform.h
    fakestringstream.h
    indent.I indent.h
    memoryBase.h
    memoryHook.h memoryHook.I
    mutexImpl.h
    mutexDummyImpl.h mutexDummyImpl.I
    mutexPosixImpl.h mutexPosixImpl.I
    mutexWin32Impl.h mutexWin32Impl.I
    mutexSpinlockImpl.h mutexSpinlockImpl.I
    nearly_zero.h
    neverFreeMemory.h neverFreeMemory.I
    numeric_types.h
    pstrtod.h
    register_type.I register_type.h
    selectThreadImpl.h
    stl_compares.I stl_compares.h
    typeHandle.I typeHandle.h
    typeRegistry.I typeRegistry.h
    typeRegistryNode.I typeRegistryNode.h
    typedObject.I typedObject.h
    pallocator.T pallocator.h
    pdeque.h plist.h pmap.h pset.h pvector.h
    lookup3.h
)

set(P3DTOOLBASE_SOURCE_FILES
    addHash.cxx
    atomicAdjustDummyImpl.cxx
    atomicAdjustI386Impl.cxx
    atomicAdjustPosixImpl.cxx
    atomicAdjustWin32Impl.cxx
    deletedBufferChain.cxx
    dtoolbase.cxx
    indent.cxx
    lookup3.c
    memoryBase.cxx
    memoryHook.cxx
    mutexDummyImpl.cxx
    mutexPosixImpl.cxx
    mutexWin32Impl.cxx
    mutexSpinlockImpl.cxx
    neverFreeMemory.cxx
    pstrtod.cxx
    register_type.cxx
    typeHandle.cxx
    typeRegistry.cxx typeRegistryNode.cxx
    typedObject.cxx
)

foreach(FILENAME IN LISTS P3DTOOLBASE_HEADER_FILES)
	set(P3DTOOLBASE_INSTALL_HEADERS ${P3DTOOLBASE_INSTALL_HEADERS} ${CMAKE_CURRENT_LIST_DIR}/${FILENAME})
endforeach()
foreach(FILENAME IN LISTS P3DTOOLBASE_SOURCE_FILES)
	set(P3DTOOLBASE_SOURCES ${P3DTOOLBASE_SOURCES} ${CMAKE_CURRENT_LIST_DIR}/${FILENAME})
endforeach()

add_library(p3dtoolbase
	${P3DTOOLBASE_INSTALL_HEADERS}
	${P3DTOOLBASE_SOURCES}
)
