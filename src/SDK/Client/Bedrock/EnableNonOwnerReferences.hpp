#pragma once

namespace Bedrock
{
    class EnableNonOwnerReferences
    {
    public:
        struct ControlBlock
        {
            bool mIsValid;
        };

        std::shared_ptr<ControlBlock> controlBlock; // this+0x8

        // prevent constructor by default
        EnableNonOwnerReferences& operator=(EnableNonOwnerReferences const&);

        virtual ~EnableNonOwnerReferences();
    };

    template <typename T>
    class NonOwnerPointer
    {
    public:
        std::shared_ptr<::Bedrock::EnableNonOwnerReferences::ControlBlock> mControlBlock;
        T* mPointer{};

        NonOwnerPointer(std::shared_ptr<::Bedrock::EnableNonOwnerReferences::ControlBlock> cb, T* p)
            : mControlBlock(std::move(cb)),
              mPointer(p)
        {
        }

        template <typename U = T>
            requires std::is_convertible_v<U*, T*>
        explicit NonOwnerPointer(U const& t)
            : NonOwnerPointer(
                static_cast<::Bedrock::EnableNonOwnerReferences const&>(t).controlBlock,
                const_cast<T*>(std::addressof(t))
            )
        {
        }

        template <typename U = T>
            requires std::is_convertible_v<U*, T*>
        explicit NonOwnerPointer(NonOwnerPointer<U> const& p) : NonOwnerPointer(
            p.mControlBlock, p.mPointer)
        {
        }

        template <typename U = T>
            requires std::is_convertible_v<U*, T*>
        explicit NonOwnerPointer(NonOwnerPointer<U>&& p)
            : NonOwnerPointer(std::move(p.mControlBlock), std::exchange(p.mPointer, nullptr))
        {
        }


        NonOwnerPointer() noexcept
        {
        }

        explicit NonOwnerPointer(std::nullptr_t) noexcept
        {
        }

        T* get() const
        {
            if (mControlBlock && mControlBlock->mIsValid)
            {
                return mPointer;
            }
            return nullptr;
        }

        explicit operator bool() const noexcept { return get() != nullptr; }

        [[nodiscard]] explicit constexpr operator T*() const { return get(); }
        [[nodiscard]] constexpr T* operator->() const { return get(); }
        [[nodiscard]] constexpr T& operator*() const { return *get(); }

        [[nodiscard]] bool operator==(nullptr_t) const noexcept { return get() == nullptr; }

        template <class U>
        [[nodiscard]] bool operator==(NonOwnerPointer<U> const& r) noexcept
        {
            return get() == r.get();
        }

        template <class U>
        [[nodiscard]] bool operator<=>(NonOwnerPointer<U> const& r) noexcept
        {
            return get() <=> r.get();
        }
    };
}