#include "xcl/pool/pool.h"

#include <cstdlib>

namespace xcl
{
    namespace
    {
        class SysPool : public Pool
        {
        public:
            void* alloc(uint64_t size) override;

            void dealloc(void* ptr, uint64_t size) override;

            void*
            realloc(void* ptr, uint64_t oldSize, uint64_t newSize) override;

        public:
            static SysPool* instance();

        private:
            SysPool() = default;

        private:
            static SysPool sysPool_;
        };

        SysPool SysPool::sysPool_ = SysPool();

        void* SysPool::alloc(uint64_t size)
        {
            return malloc(size);
        }

        void SysPool::dealloc(void* ptr, uint64_t size)
        {
            free(ptr);
        }

        void* SysPool::realloc(void* ptr, uint64_t oldSize, uint64_t newSize)
        {
            return ::realloc(ptr, newSize);
        }

        SysPool* SysPool::instance()
        {
            return &SysPool::sysPool_;
        }
    }// namespace

    Pool* Pool::sys()
    {
        return SysPool::instance();
    }
}// namespace xcl
