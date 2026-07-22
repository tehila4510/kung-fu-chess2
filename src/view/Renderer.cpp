#include "view/Renderer.h"

#include <algorithm>
#include <stdexcept>
#include <string>
#include <utility>

namespace view {
namespace {

const cv::Scalar kBlack(20, 20, 20);
const cv::Scalar kWhite(255, 255, 255);
const cv::Scalar kPanelText(220, 220, 220);
const cv::Scalar kLongRestYellow(0, 220, 255);
const cv::Scalar kShortRestBlue(255, 120, 40);
constexpr double kRestOverlayAlpha = 0.45;

void drawOverlay(Img& frame, const CellOverlay& overlay) {
    if (overlay.kind == HighlightKind::LongRest || overlay.kind == HighlightKind::ShortRest) {
        if (overlay.cell_w <= 0 || overlay.cell_h <= 0 || overlay.remaining <= 0.0) {
            return;
        }

        const double remaining = std::clamp(overlay.remaining, 0.0, 1.0);
        const int fill_h = std::max(1, static_cast<int>(overlay.cell_h * remaining));
        const int fill_y = overlay.cell_y + overlay.cell_h - fill_h;
        const cv::Scalar& color = overlay.kind == HighlightKind::LongRest
                                     ? kLongRestYellow
                                     : kShortRestBlue;
        frame.draw_filled_rect(overlay.cell_x, fill_y, overlay.cell_w, fill_h, color,
                               kRestOverlayAlpha);
        return;
    }

    if (overlay.radius <= 0) {
        return;
    }

    if (overlay.kind == HighlightKind::Capture) {
        frame.draw_solid_disc(overlay.center_x, overlay.center_y, overlay.radius, kBlack);
        const int ring_thickness = std::max(2, overlay.radius / 5);
        frame.draw_ring(overlay.center_x, overlay.center_y, overlay.radius, kWhite,
                        ring_thickness);
        return;
    }

    frame.draw_solid_disc(overlay.center_x, overlay.center_y, overlay.radius, kBlack);
}

void drawHistoryColumn(Img& frame, int panel_x, int panel_w, const char* title,
                       int score, const std::vector<std::string>& lines) {
    if (panel_w <= 0) {
        return;
    }
    frame.put_text(title, panel_x + 12, 28, 0.55, kWhite, 1);
    frame.put_text("SCORE: " + std::to_string(score), panel_x + 12, 52, 0.45,
                   kPanelText, 1);
    int y = 80;
    const int line_step = 18;
    for (const std::string& line : lines) {
        if (y > frame.height() - 12) {
            break;
        }
        frame.put_text(line, panel_x + 8, y, 0.38, kPanelText, 1);
        y += line_step;
    }
}

}  // namespace

Renderer::Renderer(Img background, std::string window_name)
    : background_(std::move(background)), window_name_(std::move(window_name)) {
    if (!background_.is_loaded()) {
        throw std::runtime_error("Renderer requires a loaded background image.");
    }
}

int Renderer::showFrame(const std::vector<PlacedSprite>& sprites,
                      const std::vector<CellOverlay>& overlays, int wait_ms,
                      const std::string& banner_text,
                      const HistoryHud& history, const UiChrome& ui) const {
    Img frame = background_.clone();

    for (const CellOverlay& overlay : overlays) {
        drawOverlay(frame, overlay);
    }

    for (const PlacedSprite& sprite : sprites) {
        if (sprite.image == nullptr || !sprite.image->is_loaded()) {
            throw std::runtime_error("Renderer received an unloaded sprite.");
        }
        sprite.image->draw_on(frame, sprite.x, sprite.y);
    }

    if (history.panel_width > 0 && history.board_width > 0) {
        drawHistoryColumn(frame, 0, history.panel_width, "WHITE",
                          history.white_score, history.white_lines);
        drawHistoryColumn(frame, history.panel_width + history.board_width,
                          history.panel_width, "BLACK", history.black_score,
                          history.black_lines);
    }

    // Status sits in a top strip over the board (or full width if no panels),
    // never inside the WHITE/BLACK history columns.
    if (!ui.status_line.empty()) {
        const int bar_h = 28;
        const int bar_y = 4;
        int bar_x = 0;
        int bar_w = frame.width();
        if (history.panel_width > 0 && history.board_width > 0) {
            bar_x = history.panel_width;
            bar_w = history.board_width;
        }
        frame.draw_filled_rect(bar_x, bar_y, bar_w, bar_h, kBlack, 0.7);
        frame.put_text_centered_in_rect(ui.status_line, bar_x, bar_y, bar_w, bar_h,
                                        0.42, kWhite, 1);
    }

    if (ui.show_play_button && ui.play_w > 0 && ui.play_h > 0) {
        const cv::Scalar playFill(40, 140, 40);
        frame.draw_filled_rect(ui.play_x, ui.play_y, ui.play_w, ui.play_h, playFill,
                               0.85);
        frame.put_text_centered_in_rect("PLAY", ui.play_x, ui.play_y, ui.play_w,
                                        ui.play_h, 0.7, kWhite, 2);
    }

    if (!banner_text.empty()) {
        if (history.panel_width > 0 && history.board_width > 0) {
            frame.draw_filled_rect(history.panel_width, 0, history.board_width,
                                   frame.height(), kBlack, 0.55);
            const double font_size = std::max(1.2, history.board_width / 280.0);
            const int thickness = std::max(2, history.board_width / 200);
            frame.put_text_centered_in_rect(banner_text, history.panel_width, 0,
                                            history.board_width, frame.height(),
                                            font_size, kWhite, thickness);
        } else {
            frame.draw_filled_rect(0, 0, frame.width(), frame.height(), kBlack, 0.55);
            const double font_size = std::max(1.2, frame.width() / 280.0);
            const int thickness = std::max(2, frame.width() / 200);
            frame.put_text_centered(banner_text, font_size, kWhite, thickness);
        }
    }

    return frame.display(window_name_, wait_ms);
}

bool Renderer::isOpen() const {
    return Img::isWindowOpen(window_name_);
}

void Renderer::close() const {
    Img::destroyWindows();
}

}  // namespace view
