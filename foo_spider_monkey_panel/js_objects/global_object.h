#pragma once

#pragma warning( push )  
#pragma warning( disable : 4251 ) // dll interface warning
#pragma warning( disable : 4996 ) // C++17 deprecation warning
#include <jsapi.h>
#pragma warning( pop )  

#include <shared_mutex>
#include <optional>
#include <set>


class js_panel_window;
class ActiveX;

namespace mozjs
{

class IHeapUser
{
public:
    IHeapUser()
    {

    }
    virtual ~IHeapUser()
    {

    }
    virtual void DisableHeapCleanup() = 0;
};

class JsContainer;
class JsGdiFont;

class JsGlobalObject
{
public:
    ~JsGlobalObject();

    static JSObject* Create( JSContext* cx, JsContainer &parentContainer, js_panel_window& parentPanel );

    static const JSClass& GetClass();

public:
    void Fail( pfc::string8_fast errorText);

public: // proto
    template <typename T>
    typename std::enable_if<std::is_same<T, ActiveX>::value, JSObject*>::type
    static GetPrototype()
    {
        assert( protoSlots_.count( activeX_protoSlot_ ) );
        return GetReservedSlot( activeX_protoSlot_ );
    }

    template <typename T>
    typename std::enable_if<std::is_same<T, ActiveX>::value, JSObject*>::type
        GetPrototype()
    {
        assert( protoSlots_.count(activeX_protoSlot_) );
        return GetReservedSlot( activeX_protoSlot_ );
    }

    template <typename T>
    typename std::enable_if<std::is_same<T, JsGdiFont>::value, JSObject*>::type
        GetPrototype()
    {
        assert( protoSlots_.count( gdiFont_protoSlot_ ) );
        return GetReservedSlot( gdiFont_protoSlot_ );
    }

    void AddPrototype( JS::HandleObject prototype );


public: // heap
    void RegisterHeapUser( IHeapUser* heapUser );
    void UnregisterHeapUser( IHeapUser* heapUser );

    uint32_t JsGlobalObject::StoreToHeap( JS::HandleValue valueToStore );
    JS::Heap<JS::Value>& GetFromHeap( uint32_t id );
    void RemoveFromHeap( uint32_t id );

    void RemoveHeapTracer();

public: // methods
    std::optional<std::nullptr_t> IncludeScript( const pfc::string8_fast& path );

private:
    JsGlobalObject( JSContext* cx, JsContainer &parentContainer, js_panel_window& parentPanel );
    JsGlobalObject( const JsGlobalObject& ) = delete;   
    JsGlobalObject& operator=( const JsGlobalObject& ) = delete;

    static void TraceHeapValue( JSTracer *trc, void *data );

private:
    JSContext * pJsCtx_ = nullptr;;
    JsContainer &parentContainer_;
    js_panel_window& parentPanel_;

private: // proto

    size_t activeX_protoSlot_ = 0;
    size_t gdiFont_protoSlot_ = 0;

    std::set<size_t> protoSlots_;
    
private: // heap
    uint32_t currentHeapId_ = 0;    
    struct HeapElement
    {
        HeapElement( JS::HandleValue inValue )
            : inUse( true )
            , value( inValue )
        {
        }

        bool inUse;
        JS::Heap<JS::Value> value;
    };
    std::mutex heapElementsLock_;
    std::unordered_map<uint32_t, std::shared_ptr<HeapElement>> heapElements_;

    std::mutex heapUsersLock_;
    std::unordered_map<IHeapUser*, IHeapUser*> heapUsers_;
};

}
