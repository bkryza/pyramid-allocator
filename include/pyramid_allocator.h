#include <cassert>
#include <cstdint>
#include <deque>
#include <vector>
#include <cmath>

namespace pyramid_allocator {

namespace detail {
unsigned int next_po2(unsigned int v)
{
    if (v % 2 == 0)
        return v;

    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;

    return v;
}
};

template <typename T> class allocator {

    class layer {
    public:
        explicit layer(std::size_t layer_size, std::size_t block_size)
            : layer_size_{layer_size}
            , block_size_{block_size}
        {
            const auto block_count = layer_size_ / block_size_;

            data_ = new std::uint8_t[layer_size_];

            free_blocks_.reserve(block_count);

            for (auto i{0U}; i < block_count; i++)
                free_blocks_.push_back(i);

            assert(data_ != nullptr);
            assert(free_blocks_.size() == block_count);
        }

        ~layer()
        {
            if (data_)
                delete[] data_;
        }

        std::size_t get_block_size() const { return block_size_; }

        T *get_block()
        {
            if (!free_blocks_.empty()) {
                auto block_number = free_blocks_.back();
                free_blocks_.pop_back();
                return reinterpret_cast<T *>(data_ + block_number);
            }

            return nullptr;
        }

        void return_block(T *address)
        {
            const auto block_number =
                reinterpret_cast<std::uint8_t *>(address) - data_;
            free_blocks_.push_back(block_number);
        }

    private:
        const std::size_t layer_size_;
        const std::size_t block_size_;
        std::uint8_t *data_;

        std::vector<std::uint32_t> free_blocks_;
    };

public:
    using value_type = T;
    using pointer = T *;
    using size_type = std::size_t;
    using const_pointer = const T *;
    using reference = T &;
    using const_reference = const T &;

    explicit allocator(std::size_t min_block_size, std::size_t max_block_size,
        std::size_t max_block_count)
    {
        const auto layer_size = max_block_size * max_block_count;

        assert(max_block_size >= min_block_size);
        assert(max_block_size % 2 == 0);

        if (layers_.empty()) {
            layers_.reserve(256);
            auto block_size = min_block_size;
            while (block_size <= max_block_size) {
                layers_.emplace_back(layer_size, block_size);

                block_size *= 2;
            }

            assert(layers_.front().get_block_size() == min_block_size);
            assert(layers_.back().get_block_size() == max_block_size);
        }
    }

    T *allocate(std::size_t count)
    {
        // Select layer
        const unsigned int block_size = count * sizeof(T);

        const auto block_size_p2 = detail::next_po2(block_size);

        if (block_size_p2 <= layers_.front().get_block_size())
            return layers_.front().get_block();

        if (block_size_p2 > layers_.back().get_block_size())
            return new T[count];

        return layers_[std::log2(block_size_p2) -
            std::log2(layers_.front().get_block_size())]
            .get_block();

        for (auto &l : layers_) {
            if (block_size <= l.get_block_size())
                return l.get_block();
        }

        return nullptr;
    }

    void deallocate(T *ptr, std::size_t count)
    {
        const std::size_t block_size = count * sizeof(T);
        for (auto &l : layers_) {
            if (block_size <= l.get_block_size())
                return l.return_block(ptr);
        }
    }

private:
    static std::vector<layer> layers_;
};
};