class IndexedSprite
{
public:
  IndexedSprite(int16_t x, uint16_t y, uint8_t *bitmap, uint16_t *palette, int16_t w, int16_t h, int16_t x_skip, bool h_loop, bool v_loop, size_t frames, int16_t speed_divider)
      : _x(x), _y(y), _bitmap(bitmap), _palette(palette), _w(w), _h(h), _x_skip(x_skip), _h_loop(h_loop), _v_loop(v_loop), _frames(frames), _speed_divider(speed_divider)
  {
  }
  IndexedSprite(int16_t x, uint16_t y, uint8_t *bitmap, uint16_t *palette, int16_t w, int16_t h, int16_t x_skip, bool h_loop, bool v_loop, size_t frames, int16_t speed_divider, uint8_t chroma_key)
      : _x(x), _y(y), _bitmap(bitmap), _palette(palette), _w(w), _h(h), _x_skip(x_skip), _h_loop(h_loop), _v_loop(v_loop), _frames(frames), _speed_divider(speed_divider), _chroma_key(chroma_key)
  {
    _has_chroma_key = true;
  }

  void set_x(int16_t x)
  {
    _x = x;
  }

  void h_scroll(int16_t val)
  {
    h_scroll(val, _w);
  }

  void h_scroll(int16_t val, int16_t bound)
  {
    _curr_sub_h_scroll += val;
    while (_curr_sub_h_scroll > _speed_divider)
    {
      ++_x;
      _curr_sub_h_scroll -= _speed_divider;
    }
    while (_curr_sub_h_scroll < -_speed_divider)
    {
      --_x;
      _curr_sub_h_scroll += _speed_divider;
    }
    if (_x < -(_w))
    {
      _x = bound;
    }
    else if (_x > bound)
    {
      _x = -(_w);
    }
  }

  void v_scroll(int16_t val)
  {
    v_scroll(val, _h);
  }

  void v_scroll(int16_t val, int16_t bound)
  {
    _curr_sub_v_scroll += val;
    while (_curr_sub_v_scroll > _speed_divider)
    {
      ++_y;
      _curr_sub_v_scroll -= _speed_divider;
    }
    while (_curr_sub_v_scroll < -_speed_divider)
    {
      --_y;
      _curr_sub_v_scroll += _speed_divider;
    }
    if (_y < -(_h))
    {
      _y = bound;
    }
    else if (_y > bound)
    {
      _y = -(_h);
    }
  }

  void next_frame()
  {
    _curr_sub_frame++;
    if (_curr_sub_frame > _speed_divider)
    {
      ++_curr_frame;
      _curr_sub_frame -= _speed_divider;
    }
    if (_curr_frame >= _frames)
    {
      _curr_frame = 0;
    }
  }

  void draw(Arduino_GFX *gfx)
  {
    if (_has_chroma_key)
    {
      gfx->drawIndexedBitmap(_x, _y, _bitmap + (_curr_frame * _w), _palette, _chroma_key, _w, _h, _x_skip);
      if (_h_loop)
      {
        gfx->drawIndexedBitmap((_x < 0) ? (_x + _w) : (_x - _w), _y, _bitmap + (_curr_frame * _w), _palette, _chroma_key, _w, _h, _x_skip);
      }
      else if (_v_loop)
      {
        gfx->drawIndexedBitmap(_x, (_y < 0) ? (_y + _h) : (_y - _h), _bitmap + (_curr_frame * _w), _palette, _chroma_key, _w, _h, _x_skip);
      }
    }
    else
    {
      gfx->drawIndexedBitmap(_x, _y, _bitmap + (_curr_frame * _w), _palette, _w, _h, _x_skip);
      if (_h_loop)
      {
        gfx->drawIndexedBitmap((_x < 0) ? (_x + _w) : (_x - _w), _y, _bitmap + (_curr_frame * _w), _palette, _w, _h, _x_skip);
      }
      else if (_v_loop)
      {
        gfx->drawIndexedBitmap(_x, (_y < 0) ? (_y + _h) : (_y - _h), _bitmap + (_curr_frame * _w), _palette, _w, _h, _x_skip);
      }
    }
  }

private:
  int16_t _x;
  int16_t _y;
  uint16_t *_palette;
  uint8_t *_bitmap;
  int16_t _w;
  int16_t _h;
  int16_t _x_skip;
  bool _h_loop;
  bool _v_loop;
  uint8_t _chroma_key;
  bool _has_chroma_key = false;
  size_t _frames;
  int16_t _speed_divider;
  size_t _curr_frame = 0;
  int16_t _curr_sub_h_scroll = 0;
  int16_t _curr_sub_v_scroll = 0;
  int16_t _curr_sub_frame = 0;
};
