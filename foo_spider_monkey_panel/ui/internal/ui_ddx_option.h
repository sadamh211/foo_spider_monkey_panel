#pragma once

#include <ui/internal/ui_ddx.h>
#include <utils/option_wrap.h>

namespace smp::ui
{

class IUiDdxOption
{
public:
    IUiDdxOption() = default;
    virtual ~IUiDdxOption() = default;

    virtual IUiDdx& Ddx() = 0;
    virtual const IUiDdx& Ddx() const = 0;
    virtual IOptionWrap& Option() = 0;
    virtual const IOptionWrap& Option() const = 0;
};

template <template <typename> typename DdxT, typename OptionT>
class UiDdxOption final
    : public IUiDdxOption
{
public:
    template <typename... Args>
    UiDdxOption( OptionT& option, Args&&... args )
        : option_( option )
        , ddx_( option, std::forward<Args>( args )... )

    {
        static_assert( std::is_base_of_v<IOptionWrap, OptionT> );
    }
    ~UiDdxOption() override = default;

    IUiDdx& Ddx() override
    {
        return ddx_;
    }
    const IUiDdx& Ddx() const override
    {
        return ddx_;
    }
    IOptionWrap& Option() override
    {
        return option_;
    }
    const IOptionWrap& Option() const override
    {
        return option_;
    }

private:
    OptionT& option_;
    DdxT<OptionT> ddx_;
};

template <template <typename> typename DdxT, typename OptionT, typename... Args>
std::unique_ptr<IUiDdxOption> CreateUiDdxOption( OptionT& value, Args&&... args )
{
    return std::make_unique<UiDdxOption<DdxT, OptionT>>( value, std::forward<Args>( args )... );
}

} // namespace smp::ui
