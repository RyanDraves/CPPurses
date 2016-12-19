#ifndef NCURSES_PAINT_ENGINE_HPP
#define NCURSES_PAINT_ENGINE_HPP

#include "../attribute.hpp"
#include "../color.hpp"
#include "../paint_engine.hpp"

#include <string>

namespace mcurses {
namespace detail {

class NCurses_paint_engine : public Paint_engine {
   public:
    NCurses_paint_engine();
    ~NCurses_paint_engine() override;

    void set_rgb(Color c, int r, int g, int b) override;
    void show_cursor() override;
    void hide_cursor() override;
    unsigned screen_width() override;
    unsigned screen_height() override;

   protected:
    void move(unsigned x, unsigned y) override;
    void put_string(const std::string& s) override;

    void set_attribute(Attribute attr) override;
    void clear_attributes() override;
    void set_background_color(Color c) override;
    void set_foreground_color(Color c) override;

    void refresh() override;

   private:
    void initialize_color_pairs() const;

    int find_pair(Color foreground, Color background) const {
        return (this->translate_(foreground) - 240) * 16 +
               (this->translate_(background) - 240);
    }

    static int translate_(Color c) { return static_cast<int>(c) + 240; }

    static int attr_to_int(Attribute attr);
    Color current_foreground();
    Color current_background();
};

}  // namespace detail
}  // namespace mcurses

#endif  // NCURSES_PAINT_ENGINE_HPP