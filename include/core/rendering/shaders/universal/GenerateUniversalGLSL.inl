

// WARNING: DO NOT INCLUDE THIS FILE ON ITS OWN.
// It's intended owner is UniversalUniforms.hpp.

// 1. Plain constexpr function (NO constexpr on the returned std::string)
constexpr size_t buildGLSLBlockString() {
    std::string ret = "layout(std140) uniform ";
    ret += universal_block_header; // Replace with your actual header variable/literal
    ret += " {\n";
    constexpr auto ctx = std::meta::access_context::unchecked();
    template for (constexpr std::meta::info m : std::define_static_array(std::meta::nonstatic_data_members_of(^^UniversalUniforms, ctx))) {
        if constexpr (std::meta::is_public(m)) {
            constexpr auto type_info = std::meta::type_of(m);
            std::string type_name(std::meta::display_string_of(type_info));

            if (type_name.contains("mat<4")) type_name = "mat4";
            if (type_name.contains("vec<4")) type_name = "vec4";
            if (type_name.contains("vec<3")) type_name = "vec3";
            if (type_name.contains("vec<2")) type_name = "vec2";
            if (type_name.contains("float")) type_name = "float";
            if (type_name.contains("unsigned int")) type_name = "uint";

            ret += "    " + type_name + " pg_" + std::string(std::meta::identifier_of(m)) + ";\n";
        }
    }
    ret += "};";
    return ret.size();
}

constexpr std::string buildGLSLBlockStringStr() {
    std::string ret = "layout(std140) uniform ";
    ret += universal_block_header; // Replace with your actual header variable/literal
    ret += " {\n";
    constexpr auto ctx = std::meta::access_context::unchecked();
    template for (constexpr std::meta::info m : std::define_static_array(std::meta::nonstatic_data_members_of(^^UniversalUniforms, ctx))) {
        if constexpr (std::meta::is_public(m)) {
            constexpr auto type_info = std::meta::type_of(m);
            std::string type_name(std::meta::display_string_of(type_info));

            if (type_name.contains("mat<4")) type_name = "mat4";
            if (type_name.contains("vec<4")) type_name = "vec4";
            if (type_name.contains("vec<3")) type_name = "vec3";
            if (type_name.contains("vec<2")) type_name = "vec2";
            if (type_name.contains("float")) type_name = "float";
            if (type_name.contains("unsigned int")) type_name = "uint";

            ret += "    " + type_name + " pg_" + std::string(std::meta::identifier_of(m)) + ";\n";
        }
    }
    ret += "};";
    return ret;
}

// 2. Helper to copy string contents safely into a fixed array
template<size_t N>
constexpr auto to_char_array(const std::string& src) {
    std::array<char, N + 1> arr{};
    std::copy_n(src.data(), N, arr.data());
    arr[N] = '\0'; // Explicit null termination
    return arr;
}

// 3. Consteval gateway to enforce compile-time running and size extraction
consteval auto generateGLSLBlock() {
    // FIX HERE: Do NOT mark this variable as constexpr!
    // It is evaluated at compile time because the surrounding function is consteval.
    constexpr auto str_size = buildGLSLBlockString();

    std::string boopsy = buildGLSLBlockStringStr();
    return to_char_array<str_size>(boopsy);
}
