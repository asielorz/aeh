// INTENTIONALLY NOT PUT #pragma once
// IT IS NEEDED AT CERTAIN PLACES TO INCLUDE IT TWICE IN THE SAME FILE

// Virtual Keys, Standard Set
XMACRO_INPUT_KEY(backspace, 0),
XMACRO_INPUT_KEY(tab, 1),

XMACRO_INPUT_KEY(clear, 2),
XMACRO_INPUT_KEY(enter, 3),

XMACRO_INPUT_KEY(shift, 4),
XMACRO_INPUT_KEY(control, 5),
XMACRO_INPUT_KEY(alt, 6),
XMACRO_INPUT_KEY(pause, 7),
XMACRO_INPUT_KEY(caps_lock, 8),
XMACRO_INPUT_KEY(kana, 9),
XMACRO_INPUT_KEY(hangul, 10),
XMACRO_INPUT_KEY(junja, 11),
XMACRO_INPUT_KEY(final_, 12),
XMACRO_INPUT_KEY(hanja, 13),
XMACRO_INPUT_KEY(kanji, 14),
XMACRO_INPUT_KEY(escape, 15),
XMACRO_INPUT_KEY(convert, 16),
XMACRO_INPUT_KEY(nonconvert, 17),
XMACRO_INPUT_KEY(space, 18),
XMACRO_INPUT_KEY(page_up, 19),
XMACRO_INPUT_KEY(page_down, 20),
XMACRO_INPUT_KEY(end, 21),
XMACRO_INPUT_KEY(home, 22),
XMACRO_INPUT_KEY(arrow_left, 23),
XMACRO_INPUT_KEY(arrow_up, 24),
XMACRO_INPUT_KEY(arrow_right, 25),
XMACRO_INPUT_KEY(arrow_down, 26),
XMACRO_INPUT_KEY(select, 27),
XMACRO_INPUT_KEY(print, 28),
XMACRO_INPUT_KEY(execute, 29),
XMACRO_INPUT_KEY(print_screen, 30),
XMACRO_INPUT_KEY(insert, 31),
XMACRO_INPUT_KEY(del, 32),
XMACRO_INPUT_KEY(help, 33),

// ASCII
XMACRO_INPUT_KEY(top0, 34),
XMACRO_INPUT_KEY(top1, 35),
XMACRO_INPUT_KEY(top2, 36),
XMACRO_INPUT_KEY(top3, 37),
XMACRO_INPUT_KEY(top4, 38),
XMACRO_INPUT_KEY(top5, 39),
XMACRO_INPUT_KEY(top6, 40),
XMACRO_INPUT_KEY(top7, 41),
XMACRO_INPUT_KEY(top8, 42),
XMACRO_INPUT_KEY(top9, 43),
XMACRO_INPUT_KEY(a, 44),
XMACRO_INPUT_KEY(b, 45),
XMACRO_INPUT_KEY(c, 46),
XMACRO_INPUT_KEY(d, 47),
XMACRO_INPUT_KEY(e, 48),
XMACRO_INPUT_KEY(f, 49),
XMACRO_INPUT_KEY(g, 50),
XMACRO_INPUT_KEY(h, 51),
XMACRO_INPUT_KEY(i, 52),
XMACRO_INPUT_KEY(j, 53),
XMACRO_INPUT_KEY(k, 54),
XMACRO_INPUT_KEY(l, 55),
XMACRO_INPUT_KEY(m, 56),
XMACRO_INPUT_KEY(n, 57),
XMACRO_INPUT_KEY(o, 58),
XMACRO_INPUT_KEY(p, 59),
XMACRO_INPUT_KEY(q, 60),
XMACRO_INPUT_KEY(r, 61),
XMACRO_INPUT_KEY(s, 62),
XMACRO_INPUT_KEY(t, 63),
XMACRO_INPUT_KEY(u, 64),
XMACRO_INPUT_KEY(v, 65),
XMACRO_INPUT_KEY(w, 66),
XMACRO_INPUT_KEY(x, 67),
XMACRO_INPUT_KEY(y, 68),
XMACRO_INPUT_KEY(z, 69),

XMACRO_INPUT_KEY(left_windows, 70),
XMACRO_INPUT_KEY(right_windows, 71),
XMACRO_INPUT_KEY(apps, 72),

XMACRO_INPUT_KEY(sleep, 73),

XMACRO_INPUT_KEY(numpad0, 74),
XMACRO_INPUT_KEY(numpad1, 75),
XMACRO_INPUT_KEY(numpad2, 76),
XMACRO_INPUT_KEY(numpad3, 77),
XMACRO_INPUT_KEY(numpad4, 78),
XMACRO_INPUT_KEY(numpad5, 79),
XMACRO_INPUT_KEY(numpad6, 80),
XMACRO_INPUT_KEY(numpad7, 81),
XMACRO_INPUT_KEY(numpad8, 82),
XMACRO_INPUT_KEY(numpad9, 83),

XMACRO_INPUT_KEY(multiply, 84),
XMACRO_INPUT_KEY(add, 85),
XMACRO_INPUT_KEY(separator, 86),
XMACRO_INPUT_KEY(subtract, 87),
XMACRO_INPUT_KEY(decimal, 88),
XMACRO_INPUT_KEY(divide, 89),
XMACRO_INPUT_KEY(f1, 90),
XMACRO_INPUT_KEY(f2, 91),
XMACRO_INPUT_KEY(f3, 92),
XMACRO_INPUT_KEY(f4, 93),
XMACRO_INPUT_KEY(f5, 94),
XMACRO_INPUT_KEY(f6, 95),
XMACRO_INPUT_KEY(f7, 96),
XMACRO_INPUT_KEY(f8, 97),
XMACRO_INPUT_KEY(f9, 98),
XMACRO_INPUT_KEY(f10, 99),
XMACRO_INPUT_KEY(f11, 100),
XMACRO_INPUT_KEY(f12, 101),
XMACRO_INPUT_KEY(f13, 102),
XMACRO_INPUT_KEY(f14, 103),
XMACRO_INPUT_KEY(f15, 104),
XMACRO_INPUT_KEY(f16, 105),
XMACRO_INPUT_KEY(f17, 106),
XMACRO_INPUT_KEY(f18, 107),
XMACRO_INPUT_KEY(f19, 108),
XMACRO_INPUT_KEY(f20, 109),
XMACRO_INPUT_KEY(f21, 110),
XMACRO_INPUT_KEY(f22, 111),
XMACRO_INPUT_KEY(f23, 112),
XMACRO_INPUT_KEY(f24, 113),

XMACRO_INPUT_KEY(num_lock, 114),
XMACRO_INPUT_KEY(scroll_lock, 115),

// NEC PC-9800 keyboard definitions
XMACRO_INPUT_KEY(numpad_equal, 116), // '' key on numpad

// Fujitsu/OASYS keyboard definitions
XMACRO_INPUT_KEY(jisho, 117),
XMACRO_INPUT_KEY(masshou, 118),
XMACRO_INPUT_KEY(left_oyayubi, 119),
XMACRO_INPUT_KEY(right_oyayubi, 120),

XMACRO_INPUT_KEY(left_shift, 121),
XMACRO_INPUT_KEY(right_shift, 122),
XMACRO_INPUT_KEY(left_control, 123),
XMACRO_INPUT_KEY(right_control, 124),
XMACRO_INPUT_KEY(left_alt, 125),
XMACRO_INPUT_KEY(right_alt, 126),
XMACRO_INPUT_KEY(alt_gr, 127 /* Same value as right_alt */), // Spanish keyboard

XMACRO_INPUT_KEY(browser_back, 128),
XMACRO_INPUT_KEY(browser_forward, 129),
XMACRO_INPUT_KEY(browser_refresh, 130),
XMACRO_INPUT_KEY(browser_stop, 131),
XMACRO_INPUT_KEY(browser_search, 132),
XMACRO_INPUT_KEY(browser_favorites, 133),
XMACRO_INPUT_KEY(browser_browser_home, 134),

XMACRO_INPUT_KEY(volume_mute, 135),
XMACRO_INPUT_KEY(volume_down, 136),
XMACRO_INPUT_KEY(volume_up, 137),

XMACRO_INPUT_KEY(media_next_track, 138),
XMACRO_INPUT_KEY(media_prev_track, 139),
XMACRO_INPUT_KEY(media_stop, 140),
XMACRO_INPUT_KEY(media_play_pause, 141),

XMACRO_INPUT_KEY(launch_mail, 142),
XMACRO_INPUT_KEY(launch_media_select, 143),
XMACRO_INPUT_KEY(launch_app1, 144),
XMACRO_INPUT_KEY(launch_app2, 145),

XMACRO_INPUT_KEY(colon_semi_colon, 146),						// ';:' for US
XMACRO_INPUT_KEY(plus, 147),					// '+' any country
XMACRO_INPUT_KEY(comma, 148),					// '),' any country
XMACRO_INPUT_KEY(minus, 149),					// '-' any country
XMACRO_INPUT_KEY(period, 150),					// '.' any country
XMACRO_INPUT_KEY(slash_question_mark, 151),						// '/?' for US
XMACRO_INPUT_KEY(grave_accent_tilde, 152),						// '`~' for US
XMACRO_INPUT_KEY(open_square_curly_brace, 153),						// '[{' for US
XMACRO_INPUT_KEY(backslash_vertical_bar, 154),						// '\|' for US
XMACRO_INPUT_KEY(close_square_curly_brace, 155),						// ']}' for US
XMACRO_INPUT_KEY(single_double_quote, 156),						// ''"' for US
XMACRO_INPUT_KEY(oem_8, 157),	// TODO: Research which key this is

// Various extended or enhanced keyboards
XMACRO_INPUT_KEY(oem_ax, 158),						//  'AX' key on Japanese AX keyboard
XMACRO_INPUT_KEY(oem_102, 159),					//  "<>" or "\|" on RT 102-key keyboard
XMACRO_INPUT_KEY(ico_help, 160),					//  Help key on ICO
XMACRO_INPUT_KEY(ico_00, 161),						//  00 key on ICO

XMACRO_INPUT_KEY(process_key, 162),

XMACRO_INPUT_KEY(packet, 163),

// Nokia/Ericsson definitions
XMACRO_INPUT_KEY(oem_reset, 164),
XMACRO_INPUT_KEY(oem_jump, 165),
XMACRO_INPUT_KEY(oem_pa1, 166),
XMACRO_INPUT_KEY(oem_pa2, 167),
XMACRO_INPUT_KEY(oem_pa3, 168),
XMACRO_INPUT_KEY(oem_wsctrl, 169),
XMACRO_INPUT_KEY(oem_cusel, 170),
XMACRO_INPUT_KEY(oem_attn, 171),
XMACRO_INPUT_KEY(oem_finish, 172),
XMACRO_INPUT_KEY(oem_copy, 173),
XMACRO_INPUT_KEY(oem_auto, 174),
XMACRO_INPUT_KEY(oem_enlw, 175),
XMACRO_INPUT_KEY(oem_backtab, 176),
XMACRO_INPUT_KEY(attn, 177),
XMACRO_INPUT_KEY(crsel, 178),
XMACRO_INPUT_KEY(exsel, 179),
XMACRO_INPUT_KEY(ereof, 180),
XMACRO_INPUT_KEY(play, 181),
XMACRO_INPUT_KEY(zoom, 182),
XMACRO_INPUT_KEY(nomame, 183),
XMACRO_INPUT_KEY(pa1, 184),
XMACRO_INPUT_KEY(oem_clear, 185),