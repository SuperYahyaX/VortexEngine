#include "LibGL.h"
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <cstdarg>  // for va_list, va_start, va_end
#include <cstdio>   // for vsnprintf
extern GLFWwindow* window;
ImDrawList* draw_list;
ImGuiIO io;


static Texture crosshairTex = { 0 };

void SetupImGuiStyle_HighContrast(Color4f Color)
{
    // DUCK RED nope! is DARK RED style by for40255 from ImThemes
    ImGuiStyle& style = ImGui::GetStyle();

    style.Alpha = 1.0f;
    style.DisabledAlpha = 0.6000000238418579f;
    style.WindowPadding = ImVec2(8.0f, 8.0f);
    style.WindowRounding = 0.0f;
    style.WindowBorderSize = 1.0f;
    style.WindowMinSize = ImVec2(32.0f, 32.0f);
    style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
    style.WindowMenuButtonPosition = ImGuiDir_Left;
    style.ChildRounding = 0.0f;
    style.ChildBorderSize = 1.0f;
    style.PopupRounding = 0.0f;
    style.PopupBorderSize = 1.0f;
    style.FramePadding = ImVec2(4.0f, 3.0f);
    style.FrameRounding = 0.0f;
    style.FrameBorderSize = 0.0f;
    style.ItemSpacing = ImVec2(8.0f, 4.0f);
    style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
    style.CellPadding = ImVec2(4.0f, 2.0f);
    style.IndentSpacing = 21.0f;
    style.ColumnsMinSpacing = 6.0f;
    style.ScrollbarSize = 14.0f;
    style.ScrollbarRounding = 0.0f;
    style.GrabMinSize = 10.0f;
    style.GrabRounding = 0.0f;
    style.TabRounding = 0.0f;
    style.TabBorderSize = 0.0f;
    style.ColorButtonPosition = ImGuiDir_Right;
    style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
    style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

    style.Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.4980392158031464f, 0.4980392158031464f, 0.4980392158031464f, 1.0f);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.03921568766236305f, 0.03921568766236305f, 0.03921568766236305f, 1.0f);
    style.Colors[ImGuiCol_ChildBg] = ImVec4(0.05490196123719215f, 0.05490196123719215f, 0.05490196123719215f, 1.0f);
    style.Colors[ImGuiCol_PopupBg] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
    style.Colors[ImGuiCol_Border] = ImVec4(Color.r, Color.g, Color.b, 1.0f);
    style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.1176470592617989f, 0.1176470592617989f, 0.1176470592617989f, 1.0f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(Color.r, Color.g, Color.b, 0.5647059082984924f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(Color.r, Color.g, Color.b, 0.5647059082984924f);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.03921568766236305f, 0.03921568766236305f, 0.03921568766236305f, 1.0f);
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.0784313753247261f, 0.0784313753247261f, 0.0784313753247261f, 0.9411764740943909f);
    style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(Color.r, Color.g, Color.b, 0.5647059082984924f);
    style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(Color.r, Color.g, Color.b, 0.501960813999176f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(Color.r, Color.g, Color.b, 1.0f);
    style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(Color.r, Color.g, Color.b, 1.0f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(Color.r, Color.g, Color.b, 1.0f);
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(Color.r, Color.g, Color.b, 0.8154506683349609f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(Color.r, Color.g, Color.b, 0.8156862854957581f);
    style.Colors[ImGuiCol_Button] = ImVec4(Color.r, Color.g, Color.b, 0.501960813999176f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(Color.r, Color.g, Color.b, 0.7450980544090271f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(Color.r, Color.g, Color.b, 1.0f);
    style.Colors[ImGuiCol_Header] = ImVec4(Color.r, Color.g, Color.b, 0.6566523313522339f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(Color.r, Color.g, Color.b, 0.8039215803146362f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(Color.r, Color.g, Color.b, 1.0f);
    style.Colors[ImGuiCol_Separator] = ImVec4(0.0784313753247261f, 0.0784313753247261f, 0.0784313753247261f, 0.501960813999176f);
    style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.0784313753247261f, 0.0784313753247261f, 0.0784313753247261f, 0.6695278882980347f);
    style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.0784313753247261f, 0.0784313753247261f, 0.0784313753247261f, 0.9570815563201904f);
    style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.1019607856869698f, 0.1137254908680916f, 0.1294117718935013f, 0.2000000029802322f);
    style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.2039215713739395f, 0.2078431397676468f, 0.2156862765550613f, 0.2000000029802322f);
    style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.3019607961177826f, 0.3019607961177826f, 0.3019607961177826f, 0.2000000029802322f);
    style.Colors[ImGuiCol_Tab] = ImVec4(Color.r, Color.g, Color.b, 0.4392156898975372f);
    style.Colors[ImGuiCol_TabHovered] = ImVec4(Color.r, Color.g, Color.b, 1.0f);
    style.Colors[ImGuiCol_TabActive] = ImVec4(Color.r, Color.g, Color.b, 1.0f);
    style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.06666667014360428f, 0.06666667014360428f, 0.06666667014360428f, 0.9725490212440491f);
    style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.06666667014360428f, 0.06666667014360428f, 0.06666667014360428f, 1.0f);
    style.Colors[ImGuiCol_PlotLines] = ImVec4(0.6078431606292725f, 0.6078431606292725f, 0.6078431606292725f, 1.0f);
    style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.9490196108818054f, 0.3450980484485626f, 0.3450980484485626f, 1.0f);
    style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.9490196108818054f, 0.3450980484485626f, 0.3450980484485626f, 1.0f);
    style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.4274509847164154f, 0.3607843220233917f, 0.3607843220233917f, 1.0f);
    style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(Color.r, Color.g, Color.b, 0.7124463319778442f);
    style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(Color.r, Color.g, Color.b, 1.0f);
    style.Colors[ImGuiCol_TableBorderLight] = ImVec4(Color.r, Color.g, Color.b, 1.0f);
    style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
    style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(0.196078434586525f, 0.196078434586525f, 0.196078434586525f, 0.6274510025978088f);
    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(Color.r, Color.g, Color.b, 1.0f);
    style.Colors[ImGuiCol_DragDropTarget] = ImVec4(0.2588235437870026f, 0.2705882489681244f, 0.3803921639919281f, 1.0f);
    style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.1803921610116959f, 0.2274509817361832f, 0.2784313857555389f, 1.0f);
    style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(Color.r, Color.g, Color.b, 0.699999988079071f);
    style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.2000000029802322f);
    style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.3499999940395355f);
}
void SetupImGuiStyle_VisualStudio()
{
    // Visual Studio style by MomoDeve from ImThemes
    ImGuiStyle& style = ImGui::GetStyle();
    style.Alpha = 1.0f;
    style.DisabledAlpha = 0.6000000238418579f;
    style.WindowPadding = ImVec2(8.0f, 8.0f);
    style.WindowRounding = 0.0f;
    style.WindowBorderSize = 1.0f;
    style.WindowMinSize = ImVec2(32.0f, 32.0f);
    style.WindowTitleAlign = ImVec2(0.0f, 0.5f);
    style.WindowMenuButtonPosition = ImGuiDir_Left;
    style.ChildRounding = 0.0f;
    style.ChildBorderSize = 1.0f;
    style.PopupRounding = 0.0f;
    style.PopupBorderSize = 1.0f;
    style.FramePadding = ImVec2(4.0f, 3.0f);
    style.FrameRounding = 0.0f;
    style.FrameBorderSize = 0.0f;
    style.ItemSpacing = ImVec2(8.0f, 4.0f);
    style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
    style.CellPadding = ImVec2(4.0f, 2.0f);
    style.IndentSpacing = 21.0f;
    style.ColumnsMinSpacing = 6.0f;
    style.ScrollbarSize = 14.0f;
    style.ScrollbarRounding = 0.0f;
    style.GrabMinSize = 10.0f;
    style.GrabRounding = 0.0f;
    style.TabRounding = 0.0f;
    style.TabBorderSize = 0.0f;
    style.ColorButtonPosition = ImGuiDir_Right;
    style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
    style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

    style.Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.5921568870544434f, 0.5921568870544434f, 0.5921568870544434f, 1.0f);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1490196138620377f, 1.0f);
    style.Colors[ImGuiCol_ChildBg] = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1490196138620377f, 1.0f);
    style.Colors[ImGuiCol_PopupBg] = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1490196138620377f, 1.0f);
    style.Colors[ImGuiCol_Border] = ImVec4(0.3058823645114899f, 0.3058823645114899f, 0.3058823645114899f, 1.0f);
    style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.3058823645114899f, 0.3058823645114899f, 0.3058823645114899f, 1.0f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.2000000029802322f, 0.2000000029802322f, 0.2156862765550613f, 1.0f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.1137254908680916f, 0.5921568870544434f, 0.9254902005195618f, 1.0f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.0f, 0.4666666686534882f, 0.7843137383460999f, 1.0f);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1490196138620377f, 1.0f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1490196138620377f, 1.0f);
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1490196138620377f, 1.0f);
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.2000000029802322f, 0.2000000029802322f, 0.2156862765550613f, 1.0f);
    style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.2000000029802322f, 0.2000000029802322f, 0.2156862765550613f, 1.0f);
    style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.321568638086319f, 0.321568638086319f, 0.3333333432674408f, 1.0f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.3529411852359772f, 0.3529411852359772f, 0.3725490272045135f, 1.0f);
    style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.3529411852359772f, 0.3529411852359772f, 0.3725490272045135f, 1.0f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.0f, 0.4666666686534882f, 0.7843137383460999f, 1.0f);
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.1137254908680916f, 0.5921568870544434f, 0.9254902005195618f, 1.0f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.0f, 0.4666666686534882f, 0.7843137383460999f, 1.0f);
    style.Colors[ImGuiCol_Button] = ImVec4(0.2000000029802322f, 0.2000000029802322f, 0.2156862765550613f, 1.0f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.1137254908680916f, 0.5921568870544434f, 0.9254902005195618f, 1.0f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.1137254908680916f, 0.5921568870544434f, 0.9254902005195618f, 1.0f);
    style.Colors[ImGuiCol_Header] = ImVec4(0.2000000029802322f, 0.2000000029802322f, 0.2156862765550613f, 1.0f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.1137254908680916f, 0.5921568870544434f, 0.9254902005195618f, 1.0f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.0f, 0.4666666686534882f, 0.7843137383460999f, 1.0f);
    style.Colors[ImGuiCol_Separator] = ImVec4(0.3058823645114899f, 0.3058823645114899f, 0.3058823645114899f, 1.0f);
    style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.3058823645114899f, 0.3058823645114899f, 0.3058823645114899f, 1.0f);
    style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.3058823645114899f, 0.3058823645114899f, 0.3058823645114899f, 1.0f);
    style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1490196138620377f, 1.0f);
    style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.2000000029802322f, 0.2000000029802322f, 0.2156862765550613f, 1.0f);
    style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.321568638086319f, 0.321568638086319f, 0.3333333432674408f, 1.0f);
    style.Colors[ImGuiCol_Tab] = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1490196138620377f, 1.0f);
    style.Colors[ImGuiCol_TabHovered] = ImVec4(0.1137254908680916f, 0.5921568870544434f, 0.9254902005195618f, 1.0f);
    style.Colors[ImGuiCol_TabActive] = ImVec4(0.0f, 0.4666666686534882f, 0.7843137383460999f, 1.0f);
    style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1490196138620377f, 1.0f);
    style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.0f, 0.4666666686534882f, 0.7843137383460999f, 1.0f);
    style.Colors[ImGuiCol_PlotLines] = ImVec4(0.0f, 0.4666666686534882f, 0.7843137383460999f, 1.0f);
    style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.1137254908680916f, 0.5921568870544434f, 0.9254902005195618f, 1.0f);
    style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.0f, 0.4666666686534882f, 0.7843137383460999f, 1.0f);
    style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.1137254908680916f, 0.5921568870544434f, 0.9254902005195618f, 1.0f);
    style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.1882352977991104f, 0.1882352977991104f, 0.2000000029802322f, 1.0f);
    style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.3098039329051971f, 0.3098039329051971f, 0.3490196168422699f, 1.0f);
    style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.2274509817361832f, 0.2274509817361832f, 0.2470588237047195f, 1.0f);
    style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.0f, 1.0f, 1.0f, 0.05999999865889549f);
    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.0f, 0.4666666686534882f, 0.7843137383460999f, 1.0f);
    style.Colors[ImGuiCol_DragDropTarget] = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1490196138620377f, 1.0f);
    style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1490196138620377f, 1.0f);
    style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 0.699999988079071f);
    style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.2000000029802322f);
    style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.1450980454683304f, 0.1450980454683304f, 0.1490196138620377f, 1.0f);
}



//Core
void GL_ImGui_Init() {
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    // Setup style
    ImGui::StyleColorsDark();
    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");
    
}
void GL_ImGui_Render() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
void GL_ImGui_Shutdown() {

    if (crosshairTex.id != 0)
    {
        GL_DestroyTexture(crosshairTex);
    }
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
//Processing
int GL_ImGui_ProcessBeforeRenderer(int style) {
    // Start ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();



    switch (style) {
    case 0:
        break;
    case 1:
        SetupImGuiStyle_VisualStudio();
        break;
    case 2:
        SetupImGuiStyle_HighContrast({1.0f,0.0f,0.0f,1.0f});
        break;
    case 3:
        SetupImGuiStyle_HighContrast({ 0.0f,1.0f,0.0f,1.0f });
        break;
    case 4:
        SetupImGuiStyle_HighContrast({ 0.0f,0.0f,1.0f,1.0f });
        break;
    default:
        break;
    }



    io = ImGui::GetIO();
    draw_list = ImGui::GetForegroundDrawList();
    return 1;
}
int GL_ImGui_BeginWindow(const char* title, Vector2 size, Vector2 pos) {
    ImVec2 impos = { pos.x,pos.y };
    ImVec2 imsize = { size.x,size.y };
    ImGui::SetNextWindowPos(impos, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(imsize, ImGuiCond_FirstUseEver);
    ImGui::Begin(title);
    return 1;

}
void GL_ImGui_Text(Vector2 pos,Color4f color, const char* fmt, ...) {
    ImGui::SetCursorPos({ pos.x, pos.y});
    va_list args;
    va_start(args, fmt);
    ImGui::TextColoredV({ color.r, color.g, color.b, color.a }, fmt, args);
    va_end(args);
}
void GL_ImGui_SliderFloat(const char* label, float value, float min, float max,Vector2 pos) {
    ImGui::SetCursorPos({ pos.x, pos.y });
    ImGui::SliderFloat(label, &value, min, max);
}
int GL_ImGui_Button(const char* label, Vector2 size, Vector2 pos) {
    ImVec2 vec22 = { size.x,size.y };
    ImGui::SetCursorPos({ pos.x, pos.y });
    return ImGui::Button(label, vec22);
}
void GL_ImGui_EndWindow() {
    ImGui::End();
}
void GL_ImGuiDrawList_Text(const char* fontPath, const char* label, Vector2 pos, Color4 color, float textSize) {
    ImFont* font;
    if (!fontPath || fontPath == NULL) {
        font = ImGui::GetFont();
    }
    else {
        font = io.Fonts->AddFontFromFileTTF(fontPath, textSize);
    }
    draw_list->AddText(ImGui::GetFont(), textSize, ImVec2(pos.x, pos.y), IM_COL32(color.r, color.g, color.b, color.a), label);
}
void GL_ImGuiDrawList_CenterText(const char* fontPath, const char* label, Color4 color, float textSize)
{
    ImGuiIO& io = ImGui::GetIO();
    ImDrawList* draw_list = ImGui::GetForegroundDrawList();
    ImFont* font = ImGui::GetFont();

    // Optional: use custom font (must already be loaded!)
    if (fontPath && fontPath[0] != '\0')
    {
        // Only load once — you must call this during ImGui setup, not here ideally
        static ImFont* customFont = NULL;
        if (!customFont)
            customFont = io.Fonts->AddFontFromFileTTF(fontPath, textSize);
        font = customFont;
    }

    // Get text size
    ImVec2 textSizeVec = font->CalcTextSizeA(textSize, FLT_MAX, 0.0f, label);

    // Center on window
    float winW = (float)GL_GetWindowWidth();
    float winH = (float)GL_GetWindowHeight();
    ImVec2 pos = ImVec2(
        (winW - textSizeVec.x) * 0.5f,
        (winH - textSizeVec.y) * 0.5f
    );

    // Convert color
    ImU32 col = IM_COL32(
        (int)(color.r * 255.0f),
        (int)(color.g * 255.0f),
        (int)(color.b * 255.0f),
        (int)(color.a * 255.0f)
    );

    draw_list->AddText(font, textSize, pos, col, label);
}

#include <string>
#include <vector>


void GL_ImGuiDrawList_Edit(
    const char* label,
    char* buffer,
    size_t buffer_size,
    const char* fontPath,
    Vector2 pos,
    Color4 textColor,
    Color4 bgColor,
    float textSize,
    int border)
{
    //ImGuiIO& io = ImGui::GetIO();

    // CRITICAL FIX: Use background draw list instead of foreground for stability
    //ImDrawList* draw_list = ImGui::GetBackgroundDrawList();

    // Backup the current font
    ImFont* oldFont = ImGui::GetFont();
    ImFont* tempFont = nullptr;

    if (fontPath && fontPath[0] != '\0')
    {
        FILE* fp = fopen(fontPath, "rb");
        if (fp)
        {
            fseek(fp, 0, SEEK_END);
            long size = ftell(fp);
            fseek(fp, 0, SEEK_SET);

            // CRITICAL FIX: Use static or persistent storage for font data
            static std::vector<unsigned char> fontDataStorage;
            fontDataStorage.resize(size);
            unsigned char* fontData = fontDataStorage.data();

            size_t bytesRead = fread(fontData, 1, size, fp);
            fclose(fp);

            if (bytesRead == (size_t)size) {
                ImFontConfig config;
                config.FontDataOwnedByAtlas = true; // Let ImGui manage the memory
                tempFont = io.Fonts->AddFontFromMemoryTTF(fontData, size, textSize, &config);

                // CRITICAL: Only build if we added a font
                if (tempFont != oldFont) {
                    io.Fonts->Build();
                }
            }
            // Note: fontDataStorage persists until program end
        }
    }

    if (!tempFont)
        tempFont = oldFont;

    // Push font for local scope
    ImGui::PushFont(tempFont);

    // CRITICAL FIX: Use proper window flags and ensure unique ID
    std::string windowId = std::string("##Edit_") + label;

    ImGui::SetNextWindowPos(ImVec2(pos.x, pos.y));
    ImGui::SetNextWindowBgAlpha(0.0f);
    ImGui::SetNextWindowSize(ImVec2(250, 40)); // Fixed size for stability

    bool windowOpen = true;
    if (ImGui::Begin(windowId.c_str(), &windowOpen,
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoBackground |
        ImGuiWindowFlags_NoFocusOnAppearing)) // CRITICAL: Prevent focus issues
    {
        // Apply custom style
        ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(bgColor.r, bgColor.g, bgColor.b, bgColor.a));
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(textColor.r, textColor.g, textColor.b, textColor.a));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, border ? 1.0f : 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f); // Add rounding for better look

        ImGui::SetNextItemWidth(200.0f);

        // CRITICAL FIX: Use proper InputText flags
        ImGuiInputTextFlags flags = ImGuiInputTextFlags_None;
        if (ImGui::InputText("##edit_field", buffer, buffer_size, flags)) {
            // Text was modified - handle if needed
        }

        ImGui::PopStyleVar(2);
        ImGui::PopStyleColor(2);
    }
    ImGui::End();

    // Restore font
    ImGui::PopFont();
}


int GL_ImGuiDrawList_CheckBox(Vector2 pos, Vector2 size, const char* label, int* checked)
{

    ImVec2 min = ImVec2(pos.x, pos.y);
    ImVec2 max = ImVec2(pos.x + size.x, pos.y + size.y);

    // Background
    ImU32 box_col = IM_COL32(40, 40, 40, 255);
    ImU32 border_col = IM_COL32(200, 200, 200, 255);
    draw_list->AddRectFilled(min, max, box_col, 3.0f);
    draw_list->AddRect(min, max, border_col, 3.0f);

    // Click handling
    if (ImGui::IsMouseClicked(0)) {
        ImVec2 mouse = io.MousePos;
        if (mouse.x >= min.x && mouse.y >= min.y && mouse.x <= max.x && mouse.y <= max.y) {
            *checked = !(*checked);
        }
    }

    // Checkmark
    if (*checked) {
        float pad = size.y * 0.2f;
        ImVec2 p1 = ImVec2(min.x + pad, min.y + size.y * 0.5f);
        ImVec2 p2 = ImVec2(min.x + size.x * 0.4f, max.y - pad);
        ImVec2 p3 = ImVec2(max.x - pad, min.y + pad);
        draw_list->AddLine(p1, p2, IM_COL32(0, 255, 0, 255), 2.5f);
        draw_list->AddLine(p2, p3, IM_COL32(0, 255, 0, 255), 2.5f);
    }

    // Label
    ImVec2 text_pos = ImVec2(max.x + 8, min.y + (size.y - ImGui::GetFontSize()) * 0.5f);
    draw_list->AddText(text_pos, IM_COL32(255, 255, 255, 255), label);

    return *checked;
}
static bool sliderActive = false; // track dragging state
void GL_ImGuiDrawList_SliderFloat(Vector2 pos, Vector2 size, float* value, float v_min, float v_max, const char* label)
{

    ImVec2 min = { pos.x, pos.y };
    ImVec2 max = { pos.x + size.x, pos.y + size.y };

    // --- Normalize value [0,1]
    float t = (*value - v_min) / (v_max - v_min);
    t = clampf(t, 0.0f, 1.0f);

    // --- Track rectangle
    ImU32 track_col = IM_COL32(60, 60, 60, 255);
    ImU32 fill_col = IM_COL32(100, 200, 250, 255);
    float barHeight = size.y * 0.35f;
    ImVec2 barMin = { min.x, min.y + (size.y - barHeight) * 0.5f };
    ImVec2 barMax = { max.x, barMin.y + barHeight };
    float barWidth = barMax.x - barMin.x;

    draw_list->AddRectFilled(barMin, barMax, track_col, 4.0f);

    // --- Filled portion
    ImVec2 fillMax = { barMin.x + t * barWidth, barMax.y };
    draw_list->AddRectFilled(barMin, fillMax, fill_col, 4.0f);

    // --- Knob
    float knobX = barMin.x + t * barWidth;
    ImVec2 knobCenter = { knobX, (barMin.y + barMax.y) * 0.5f };
    float knobRadius = size.y * 0.4f;

    draw_list->AddCircleFilled(knobCenter, knobRadius, IM_COL32(255, 255, 255, 255), 16);
    draw_list->AddCircle(knobCenter, knobRadius, IM_COL32(0, 0, 0, 255), 16, 1.5f);
    ImVec2 mouse = io.MousePos;

    // Knob hitbox is slightly bigger for comfort
    float knobHitRadius = knobRadius * 1.3f;
    bool hoverKnob = (mouse.x >= knobCenter.x - knobHitRadius && mouse.x <= knobCenter.x + knobHitRadius &&
        mouse.y >= knobCenter.y - knobHitRadius && mouse.y <= knobCenter.y + knobHitRadius);
    bool hoverBar = (mouse.x >= barMin.x && mouse.x <= barMax.x &&
        mouse.y >= barMin.y - size.y * 0.3f && mouse.y <= barMax.y + size.y * 0.3f);
    if (ImGui::IsMouseClicked(0) && (hoverKnob || hoverBar)) {
        sliderActive = true;
    }
    if (!ImGui::IsMouseDown(0)) {
        sliderActive = false;
    }
    if (sliderActive) {
        float newT = (mouse.x - barMin.x) / barWidth;
        newT = clampf(newT, 0.0f, 1.0f);
        *value = v_min + newT * (v_max - v_min);
    }
    // --- Draw label and value
    char buf[64];
    snprintf(buf, sizeof(buf), "%.3f", *value);
    ImVec2 textPos = { max.x + 8, min.y + (size.y - ImGui::GetFontSize()) * 0.5f };
    draw_list->AddText(textPos, IM_COL32(255, 255, 255, 255), label);
    ImVec2 valPos = { textPos.x + 70, textPos.y };
    draw_list->AddText(valPos, IM_COL32(200, 255, 200, 255), buf);
}
void GL_ImGuiDrawList_ImagePanel(Vector2 pos, Vector2 size, Texture tex, Color4 tint)
{
    ImVec2 min = { pos.x, pos.y };
    ImVec2 max = { pos.x + size.x, pos.y + size.y };
    ImU32 color = IM_COL32(tint.r, tint.g, tint.b, tint.a);
    draw_list->AddImage((ImTextureID)(intptr_t)tex.id, min, max, ImVec2(0, 0), ImVec2(1, 1), color);
    draw_list->AddRect(min, max, IM_COL32(255, 255, 255, 100), 0.0f, 0, 1.0f);
}
void GL_ImGuiDrawList_Crosshair(const char* pathTex, int sizeX, int sizeY)
{

    // Load texture only once
    if (crosshairTex.id == 0)
    {
        Texture crosshairTex = GL_LoadTexture(pathTex, TEXTURE_FILTER_LINEAR);
        crosshairTex = crosshairTex;
    }

    // Get screen center
    ImVec2 screenSize = ImGui::GetIO().DisplaySize;
    ImVec2 center(screenSize.x * 0.5f, screenSize.y * 0.5f);

    // Define crosshair size
    ImVec2 size((float)sizeX, (float)sizeY);

    // Get draw list for foreground (always on top)
    ImDrawList* draw_list = ImGui::GetForegroundDrawList();

    // Compute image bounds
    ImVec2 p_min(center.x - size.x * 0.5f, center.y - size.y * 0.5f);
    ImVec2 p_max(center.x + size.x * 0.5f, center.y + size.y * 0.5f);

    // Draw the crosshair image
    draw_list->AddImage(
        (void*)(intptr_t)crosshairTex.id,
        p_min, p_max,
        ImVec2(0, 0), ImVec2(1, 1),
        IM_COL32_WHITE
    );
}

//Imgui Menu bar support
int GL_ImGui_BeginMenuBar() {
    return ImGui::BeginMainMenuBar();
}
void GL_ImGui_EndMenuBar() {
    ImGui::EndMainMenuBar();
}
int GL_ImGui_BeginMenu(const char* label, int enabled) {
    return ImGui::BeginMenu(label, enabled);
}
void GL_ImGui_EndMenu() {
    ImGui::EndMenu();
}
int GL_ImGui_NewMenuItem(const char* label, const char* shortcut, int selected, int enabled) {
    return ImGui::MenuItem(label, shortcut, selected, enabled);
}
void GL_ImGui_AddSeparator() {
    ImGui::Separator();
}
void GL_ImGui_MsgBoxEx(const char* captionName,const char* message,void (*callback)(void),void (*cancelCallback)(void))
{
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(600, 250), ImGuiCond_Appearing);
    if (ImGui::BeginPopupModal(captionName, NULL, ImGuiWindowFlags_NoResize))
    {
        ImGui::Dummy(ImVec2(0, 20));
        ImGui::TextWrapped("%s", message);
        ImGui::Dummy(ImVec2(0, 20));
        ImVec2 buttonSize(135, 35);
        float windowWidth = ImGui::GetWindowSize().x;
        float spacing = 20.0f;
        float totalWidth = buttonSize.x * 2 + spacing;
        float startX = (windowWidth - totalWidth) * 0.5f;
        ImGui::SetCursorPosX(startX);
        ImGui::SetCursorPosY(ImGui::GetWindowSize().y - 60);
        if (ImGui::Button("OK", buttonSize))
        {
            if (callback)
                callback();
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine(startX + buttonSize.x + spacing);
        ImGui::SetCursorPosY(ImGui::GetWindowSize().y - 60);
        if (ImGui::Button("Cancel", buttonSize))
        {
            if (cancelCallback)
                cancelCallback();
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}
void GL_ImGui_MsgBox(const char* captionName, const char* message, void (*callback)(void))
{
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(400, 180), ImGuiCond_Appearing);

    if (ImGui::BeginPopupModal(captionName, NULL, ImGuiWindowFlags_NoResize))
    {
        ImGui::Dummy(ImVec2(0, 20));
        ImGui::TextWrapped("%s", message);
        ImGui::Dummy(ImVec2(0, 20));
        ImVec2 buttonSize(135, 35);
        float windowWidth = ImGui::GetWindowSize().x;
        ImGui::SetCursorPosX((windowWidth - buttonSize.x) * 0.5f);
        ImGui::SetCursorPosY(ImGui::GetWindowSize().y - 60);
        if (ImGui::Button("OK", buttonSize))
        {
            if (callback)
                callback();
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}
void GL_ImGui_OpenPopupMsgBox(const char* captionName) {
    ImGui::OpenPopup(captionName);
}

//FPS
static const char* TextFormat(const char* fmt, ...)
{
    char buffer[1024];

    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    return buffer;
}
void GL_ImGuiDrawList_SimpleFPS(float x, float y) {
    double fps = 1.0 / (double)GL_GetDeltaTime();
    Color4 currentColor = { 0,255,0,255 };
    if (fps >= 45.0) {
        currentColor = { 0,255,0,255 };
    }
    if (fps <= 25.0) {
        currentColor = { 255,255,0,255 };
    }
    if (fps <= 10.0) {
        currentColor = { 255,0,0,255 };
    }
    GL_ImGuiDrawList_Text(NULL, TextFormat("FPS:%d", (int)fps), { x, y }, currentColor, 16);
}







