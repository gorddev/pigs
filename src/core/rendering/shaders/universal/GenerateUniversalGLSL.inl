
#ifndef __EMSCRIPTEN__

#include <meta>

// 1. Plain constexpr function (NO constexpr on the returned std::string)
constexpr size_t GLSLBlockStrLen() {
    std::string ret = "layout(std140) uniform ";
    ret += pg::universal_block_header; // Replace with your actual header variable/literal
    ret += " {\n";
    constexpr auto ctx = std::meta::access_context::unchecked();
    template for (constexpr std::meta::info m : std::define_static_array(std::meta::nonstatic_data_members_of(^^pg::UniversalUniforms, ctx))) {
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

constexpr std::string GLSLBlockStr() {
    std::string ret = "layout(std140) uniform ";
    ret += pg::universal_block_header; // Replace with your actual header variable/literal
    ret += " {\n";
    constexpr auto ctx = std::meta::access_context::unchecked();
    template for (constexpr std::meta::info m : std::define_static_array(std::meta::nonstatic_data_members_of(^^pg::UniversalUniforms, ctx))) {
        if constexpr (std::meta::is_public(m)) {
            constexpr auto type_info = std::meta::type_of(m);
            std::string type_name(std::meta::display_string_of(type_info));

            if (type_name.contains("mat<4")) type_name = "mat4";
            else if (type_name.contains("vec<4")) type_name = "vec4";
            else if (type_name.contains("vec<3")) type_name = "vec3";
            else if (type_name.contains("vec<2")) type_name = "vec2";
            else if (type_name.contains("float")) type_name = "float";
            else if (type_name.contains("unsigned int")) type_name = "uint";
            else if (type_name.contains("int")) type_name = "int";

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
    constexpr auto str_size = GLSLBlockStrLen();

    std::string boopsy = GLSLBlockStr();
    return to_char_array<str_size>(boopsy);
}
#else
consteval auto generateGLSLBlock() {
	return
R"(layout(std140) uniform pg_SceneData {
	uniform mat4 pg_screenProjection;
	unifrom mat4 pg_resolution;
	uniform uint pg_frame;
	uniform float time;
})";
}
#endif
