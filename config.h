#include <X11/XF86keysym.h>
#include "gaplessgrid.c"
/* See LICENSE file for copyright and license details. */

#define NUMCOLORS 5
static const char colors[NUMCOLORS][MAXCOLORS][8] = {
  //border, foreground, background
	{"#005577", "#eeeeee", "#005577"}, // normal (white on light blue)
	{"#000088", "#eeeeee", "#000088"}, // selected (white on dark blue)
	{"#00ff41", "#00ff41", "#005577"}, // dark green on light blue
	{"#cc6666", "#cc6666", "#005577"}, // error (white on red)
	{"#f0c674", "#f0c674", "#005577"}, // warning (black on yellow)
	// add more here
};

/* appearance */
static const char *fonts[] = {
	"Wuncon Siji:size=12",
	"SAO UI:size=12",
	"-microsoft-comic sans ms-medium-r-normal--0-0-0-0-p-0-adobe-standard",
};
static const char dmenufont[]       = "SAO UI:size=12";
static const unsigned int borderpx  = 3;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const unsigned int systraypinning = 0;   /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static const unsigned int systrayspacing = 2;   /* systray spacing */
static const int systraypinningfailfirst = 1;   /* 1: if pinning fails, display systray on the first monitor, 0: display systray on the last monitor*/
static const int showsystray        = 1;        /* 0 means no systray */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const unsigned int gappx     = 3; 	/* gap pixel between windows */

/*   Display modes of the tab bar: never shown, always shown, shown only in */
/*   monocle mode in presence of several windows.                           */
/*   Modes after showtab_nmodes are disabled                                */
enum showtab_modes { showtab_never, showtab_auto, showtab_nmodes, showtab_always};
static const int showtab            = showtab_auto; /* Default tab bar show mode */
static const int toptab             = False;    /* False means bottom tab bar */

/* tagging */
static const char *tags[] = { "\ue1ec", "\ue1a6", "\ue1f5", "\ue181", "\ue182", "\ue183", "\ue184", "\ue185", "\ue1ee" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating	   monitor */
	{ "Gimp",     NULL,       NULL,       0,            1,         	   -1 },
	{ "Firefox",  NULL,       NULL,       1 << 8,       0,        	   -1 },
	{ "spotify",  NULL,       NULL,       1 << 1,       0,             -1 },
	{ "chromium", NULL,	      NULL,	      1 << 8,	    0,		       -1 },
	{ "vivaldi",  NULL,	      NULL,	      1 << 8,	    0,		       -1 },
	{ "Steam",    NULL,       NULL,       1 << 2,       1,		       -1 },
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 0;    /* 1 means respect size hints in tiled resizals */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "\uE002",      tile },    /* first entry is default */
	{ "\uE006",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      	 monocle },
	{ "\uE005",   	 gaplessgrid},
	{ "\uE003",	 htile},
	{ "\uE010",	 centeredmaster},
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      comboview,      {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      combotag,       {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-p","Run: ","-i", "-fn", dmenufont, "-nb", "black", "-nf", "white", "-sb", "blue", "-sf", "white", NULL };
static const char *j4dmenucmd[] = {"j4-dmenu-desktop", "--dmenu=dmenu -p 'Run: ' -i -fn 'SAO UI:size=13' -nb black -nf white -sb blue -sf white", NULL };
static const char *termcmd[]  = { "urxvt", NULL };
static const char *raisevol[] = { "ponymix", "increase", "5", NULL};
static const char *decrvol[] =  { "ponymix", "decrease", "5", NULL};
static const char *mutevol[] =  { "ponymix", "toggle", NULL};


static Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY,                       XK_p,      spawn,          {.v = dmenucmd } },
	{ MODKEY|ShiftMask,		XK_d,	   spawn,	   {.v = j4dmenucmd } },
	{ MODKEY|ShiftMask,             XK_Return, spawn,          {.v = termcmd } },
	{ MODKEY,                       XK_b,      togglebar,      {0} },
	{ MODKEY,                       XK_w,      tabmode,        {-1} },
	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_d,      incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY,                       XK_Return, zoom,           {0} },
	{ MODKEY,                       XK_Tab,    view,           {0} },
	{ MODKEY|ShiftMask,             XK_c,      killclient,     {0} },
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                       XK_f,      setlayout,      {.v = &layouts[1]} },
	{ MODKEY,                       XK_m,      setlayout,      {.v = &layouts[2]} },
	{ MODKEY,			XK_g,	   setlayout,	   {.v = &layouts[3]} },
	{ MODKEY|ShiftMask,		XK_b,	   setlayout,	   {.v = &layouts[4]} },
	{ MODKEY,			XK_c,	   setlayout,	   {.v = &layouts[5]} },
	{ MODKEY,                       XK_space,  setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
	{ MODKEY,                       XK_Left,   viewtoleft,     {0} },
	{ MODKEY,                       XK_Right,  viewtoright,    {0} },
	{ MODKEY|ShiftMask,             XK_Left,   tagtoleft,      {0} },
	{ MODKEY|ShiftMask,             XK_Right,  tagtoright,     {0} },
	{ 0,				XF86XK_AudioRaiseVolume,   spawn,	{.v = raisevol }},
	{ 0,				XF86XK_AudioLowerVolume,   spawn,	{.v = decrvol }},
	{ 0,				XF86XK_AudioMute,	   spawn,	{.v = mutevol  }},
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
	{ MODKEY|ShiftMask,             XK_q,      quit,           {0} },
};

/* button definitions */
/* click can be ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,      	{0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        combotag,       {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
	{ ClkTabBar,            0,              Button1,        focuswin,       {0} },
};
