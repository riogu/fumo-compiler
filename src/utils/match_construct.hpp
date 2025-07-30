#include <type_traits>
#include <variant>

template<typename T, typename Head, typename... Tail>
constexpr int type_index(int index = 0) {
    if constexpr (std::is_same_v<T, Head>) return index;
    else
        return type_index<T, Tail...>(index + 1);
}
template<typename... Types>
struct type_sequence {
    constexpr type_sequence(const std::variant<Types...>&) {}
    template<typename T>
    constexpr static int idx = type_index<T, Types...>();
};

#define match(v)                                                                                    \
{                                                                                                   \
    auto& v___ = v;                                                                                 \
    bool was_default = false;                                                                       \
    switch (auto t_seq___ = wrapped_type_seq(v); index_of(v)) {                                   

#define holds(T, ...)                                                                               \
    break;                                                                                          \
}                                                                                                   \
    case t_seq___.idx<std::remove_pointer<std ::remove_cvref<T>::type>::type>: {                    \
        using enum T::Kind;                                                                         \
        __VA_OPT__(T __VA_ARGS__ =                                                                  \
            get_elem<std::remove_pointer<std::remove_cvref<T>::type>::type>(v___);)

#define _default                                                                                    \
        break;                                                                                      \
    }                                                                                               \
        default:                                                                                    \
            was_default = true;                                                                     \
}                                                                                                   \
    if (was_default)

