#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <iostream>
#include <ftxui/component/component.hpp>
#include <ftxui/screen/screen.hpp>
#include <ftxui/component/screen_interactive.hpp>

#include "nvapi/NvWrapper.hh"

std::vector<NvAPI_SettingRequest> settings = {
    {ANISO_MODE_SELECTOR_ID, 1},
    {ANISO_MODE_LEVEL_ID, 16},
    {FXAA_ENABLE_ID, 0},
    {AA_MODE_GAMMACORRECTION_ID, 2},
    {AA_MODE_SELECTOR_ID, 0},
    {AA_MODE_ALPHATOCOVERAGE_ID, 0},
    // skipped CUDA, DSR, Low Latency <<<
    {FRL_FPS_ID, 0},
    // skipped monitor techonology
    // {MAXWELL_B_SAMPLE_INTERLEAVE_ID, 1}, <<<< THIS SETTING IS PROBLEMATIC FOR WEBAPPS
    // skipped OGL GDI compat & affinity gpu
    {PREFERRED_PSTATE_ID, 1},
    //
    {REFRESH_RATE_OVERRIDE_ID, 1},
    // skipped shader cache size {might be default}
    {PS_TEXFILTER_ANISO_OPTS2_ID, 0},
    {PS_TEXFILTER_NO_NEG_LODBIAS_ID, 0},
    {QUALITY_ENHANCEMENTS_ID, 20},
    {PS_TEXFILTER_DISABLE_TRILIN_SLOPE_ID, 0},
    {OGL_THREAD_CONTROL_ID, 0},
    {OGL_TRIPLE_BUFFER_ID, 0},
    // skipped: vsync {mainly application based}
    {VRPRERENDERLIMIT_ID, 1}
};

using namespace ftxui;

int main()
{
    HANDLE hCnsle = GetStdHandle(STD_OUTPUT_HANDLE);
    {
        system("cls");
        SetConsoleTextAttribute(hCnsle, 10);
        std::cout << " $$$$$$  $$$$$$  $$$$$$$$ $$ $$$$$$$ $$    $$ $$$    $$  $$$$$$ " << std::endl;
        std::cout << "$$    $$ $$   $$    $$    $$ $$       $$  $$  $$$$   $$ $$      " << std::endl;
        std::cout << "$$    $$ $$$$$$     $$    $$ $$$$$$$   $$$$   $$ $$  $$ $$      " << std::endl;
        std::cout << "$$    $$ $$         $$    $$      $$    $$    $$  $$ $$ $$      " << std::endl;
        std::cout << " $$$$$$  $$         $$    $$ $$$$$$$    $$    $$   $$$$  $$$$$$ " << std::endl;
        
        printf("\n\nBy using OptiSync, you acknowledge our disclaimer. Learn more at https://github.com/plcid/OptiSync/.\n\n");
        system("pause");
        system("cls");
    }
    SetConsoleTextAttribute(hCnsle, 15);

    int status = NVAPI_OK;

    status ^= (int)nvw::init();

    checkSafe(status);

    const int setting_count = 17;
    bool selections[setting_count];
    std::fill(selections, selections + setting_count, true);
    bool all_options_selected = true;
    {
        auto scr = ScreenInteractive::FitComponent();

        // -- Checkbox(es) -----------------------------------------------------------
        auto all_options_select = Container::Vertical({
            Checkbox("(RECOMMENDED) All Settings", &all_options_selected)
            });

        Components checkboxes = {};
        for (int i = 0; i < setting_count; i++) {
            NVDRS_SETTING stg;
            status ^= nvw::getSetting(settings[i].uid, &stg);
            checkSafe(status);
            std::string settingName = nvw::NvUnicodeStrToCStr(stg.settingName);
            checkboxes.push_back(
                Checkbox(settingName, &selections[i])
            );
        }

        auto individual_options = Container::Vertical(checkboxes);

        auto finalize_selection = Container::Vertical({
            Button("Finalize Settings to Modify", scr.ExitLoopClosure() /* exit upon click */)
        });

        // -- Layout -----------------------------------------------------------------
        auto layout = Container::Vertical({
            all_options_select,
            individual_options,
            finalize_selection
        });

        auto component = Renderer(layout, [&] {
            return vbox({
                all_options_select->Render() | border,
                individual_options->Render() | border,
                finalize_selection->Render()
                }) | xflex;
        });

        scr.Loop(component);
    }

    system("cls");
    printf("\n");

    {
        for (int i = 0; i < setting_count; i++) {
            if (selections[i] || all_options_selected) {
                status ^= nvw::setSetting(settings[i]);
                checkSafe(status);
            }
        }
    }

    checkSafe(status);

    status ^= (int)nvw::exit();

    checkSafe(status);

    printf("\nSuccessful! We Would Recommend Restarting Computer To Effect Changes.\n");
    system("pause");

    return 0;
}
