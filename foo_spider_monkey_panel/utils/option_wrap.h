#pragma once

#include <type_traits>
#include <variant>

namespace smp::internal
{

// TODO: move
template <typename, template <typename...> class>
inline constexpr bool isSpecializationOfV = false;

template <template <typename...> class T, typename... Args>
inline constexpr bool isSpecializationOfV<T<Args...>, T> = true;

// variant passes the check for equality for some reason
template <typename T>
inline constexpr bool isComparableV = !smp::internal::isSpecializationOfV<T, std::variant> && std::is_invocable_r_v<bool, std::equal_to<>, T, T>;

} // namespace smp::internal

namespace smp
{

class IOptionWrap
{
public:
    IOptionWrap() = default;
    virtual ~IOptionWrap() = default;

    virtual bool HasChanged() const = 0;
    virtual void Apply() = 0;
    virtual void Revert() = 0;
};

template <typename T>
class OptionWrap
    : public IOptionWrap
{
    template <typename OptionT, typename ValueT>
    friend class SuboptionWrap;

public:
    using value_type = typename T;

public:
    //template <std::enable_if_t<std::is_default_constructible_v<T>, int> = 0>
    OptionWrap()
        : curValue_{}
        , savedValue_{}
    {
    }

    OptionWrap( const value_type& value )
        : curValue_( value )
        , savedValue_( value )
    {
    }

    OptionWrap( const OptionWrap& ) = delete;

    OptionWrap& operator=( const value_type& value )
    {
        SetValue( value );
        return *this;
    }

    operator const value_type&() const
    {
        return GetCurrentValue();
    }

    const value_type& GetSavedValue() const
    {
        return savedValue_;
    }

    const value_type& GetCurrentValue() const
    {
        return curValue_;
    }

    void InitializeValue( const value_type& value )
    {
        curValue_ = value;
        savedValue_ = value;
        hasChanged_ = false;
    }

    void SetValue( const value_type& value, bool dontCheck = false )
    {
        if constexpr ( smp::internal::isComparableV<value_type> )
        {
            hasChanged_ = ( dontCheck ? true : ( savedValue_ != value ) );
        }
        else
        {
            hasChanged_ = true;
        }

        curValue_ = value;
    }

    // > IOptionWrap

    bool HasChanged() const override
    {
        return hasChanged_;
    }

    void Apply() override
    {
        if ( hasChanged_ )
        {
            savedValue_ = curValue_;
            hasChanged_ = false;
        }
    }

    void Revert() override
    {
        if ( hasChanged_ )
        {
            curValue_ = savedValue_;
            hasChanged_ = false;
        }
    }

    // < IOptionWrap

private:
    bool hasChanged_ = false;
    value_type savedValue_;
    value_type curValue_;
};

template <typename OptionT, typename ValueT>
class SuboptionWrap
    : public IOptionWrap
{
public:
    using value_type = typename ValueT;
    using AccessorFn = typename value_type& (*)( typename OptionT::value_type& );

    SuboptionWrap( OptionT& optionWrap, AccessorFn fn )
        : optionWrap_( optionWrap )
        , fn_( fn )
    {
    }

    SuboptionWrap& operator=( const value_type& value )
    {
        SetValue( value );
        return *this;
    }

    operator const value_type&() const
    {
        return GetCurrentValue();
    }

    // > IOptionWrap

    bool HasChanged() const override
    {
        if constexpr ( smp::internal::isComparableV<value_type> )
        {
            return ( fn_( optionWrap_.savedValue_ ) != fn_( optionWrap_.curValue_ ) );
        }
        else
        {
            return true;
        }
    }

    void Apply() override
    {
        if ( optionWrap_.hasChanged_ )
        {
            fn_( optionWrap_.savedValue_ ) = fn_( optionWrap_.curValue_ );
        }
    }

    void Revert() override
    {
        if ( optionWrap_.hasChanged_ )
        {
            fn_( optionWrap_.curValue_ ) = fn_( optionWrap_.savedValue_ );
        }
    }

    // < IOptionWrap

    void SetValue( const value_type& value )
    {
        if constexpr ( smp::internal::isComparableV<value_type> )
        {
            optionWrap_.hasChanged_ = ( fn_( optionWrap_.savedValue_ ) != value );
        }
        else
        {
            optionWrap_.hasChanged_ = true;
        }

        fn_( optionWrap_.curValue_ ) = value;
    }

    const value_type& GetSavedValue() const
    {
        return fn_( optionWrap_.savedValue_ );
    }

    const value_type& GetCurrentValue() const
    {
        return fn_( optionWrap_.curValue_ );
    }

private:
    OptionT& optionWrap_;
    AccessorFn fn_;
};

} // namespace smp
