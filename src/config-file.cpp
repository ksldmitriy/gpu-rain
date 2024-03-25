#include "application.hpp"
#include "rain-simulation.hpp"

// Application

const size_t Application::s_drops_count = 1024 * 2;

const glm::fvec2 Application::s_drop_size = glm::fvec2(3.6f, 52);
const glm::fvec2 Application::s_splash_size = glm::fvec2(30, 23);

const char *const Application::s_test_media_path = "media/main-color.png";
const glm::uvec2 Application::s_framebuffer_size = glm::uvec2(1920, 1080);

// RainSimulation

static constexpr float cam_z = -16;
static constexpr float td_cam_size = 28;

// scene
const float RainSimulation::s_min_z = -5.22811 - cam_z;
const float RainSimulation::s_max_z = RainSimulation::s_min_z + td_cam_size;
const float RainSimulation::s_hfov = glm::radians(60.f);
const float RainSimulation::s_top_down_camera_y = 9 + 6.5;

const float RainSimulation::s_near_plane = s_min_z;
const float RainSimulation::s_kill_plane = -4.5;

// drop
const float RainSimulation::s_min_drop_size = 0.2f;
const float RainSimulation::s_max_drop_size = 1.f;
const float RainSimulation::s_droplet_speed = 18.f;
const float RainSimulation::s_splash_time = 0.12f;

const float RainSimulation::s_fall_drop_alpha_coefficient = 0.7f;
const float RainSimulation::s_splash_alpha_coefficient = 0.9f;

const char *RainSimulation::s_main_depth_image_path = "media/main-depth.exr";
const char *RainSimulation::s_top_down_depth_image_path =
    "media/top-down-depth.exr";
