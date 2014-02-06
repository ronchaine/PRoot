/* This file is automatically generated from the documentation. EDIT AT YOUR OWN RISK. */

#ifndef CARE_CLI_H
#define CARE_CLI_H

#include "cli/cli.h"

#ifndef VERSION
#define VERSION "2.1"
#endif

#define CARE_MAX_SIZE 1024

static char const *default_concealed_paths[] = {
	"$HOME",
	"/tmp",
	NULL,
};

static char const *default_revealed_paths[] = {
	"$PWD",
	NULL,
};

static char const *default_volatile_paths[] = {
	"/dev",
	"/proc",
	"/sys",
	"/run/shm",
	"/tmp/.X11-unix",
	"/tmp/.ICE-unix",
	"$XAUTHORITY",
	"$ICEAUTHORITY",
	"/var/run/dbus/system_bus_socket",
	"/var/tmp/kdecache-$LOGNAME",
	NULL,
};

static char const *default_volatile_envars[] = {
	"DISPLAY",
	"http_proxy",
	"https_proxy",
	"ftp_proxy",
	"all_proxy",
	"HTTP_PROXY",
	"HTTPS_PROXY",
	"FTP_PROXY",
	"ALL_PROXY",
	"DBUS_SESSION_BUS_ADDRESS",
	"SESSION_MANAGER",
	"XDG_SESSION_COOKIE",
	NULL,
};

static int handle_option_o(Tracee *tracee, const Cli *cli, char *value);
static int handle_option_c(Tracee *tracee, const Cli *cli, char *value);
static int handle_option_r(Tracee *tracee, const Cli *cli, char *value);
static int handle_option_p(Tracee *tracee, const Cli *cli, char *value);
static int handle_option_e(Tracee *tracee, const Cli *cli, char *value);
static int handle_option_m(Tracee *tracee, const Cli *cli, char *value);
static int handle_option_d(Tracee *tracee, const Cli *cli, char *value);
static int handle_option_v(Tracee *tracee, const Cli *cli, char *value);
static int handle_option_V(Tracee *tracee, const Cli *cli, char *value);
static int handle_option_x(Tracee *tracee, const Cli *cli, char *value);
static int handle_option_h(Tracee *tracee, const Cli *cli, char *value);

static int pre_initialize_bindings(Tracee *, const Cli *, size_t, char *const *, size_t);
static int post_initialize_bindings(Tracee *, const Cli *, size_t, char *const *, size_t);

static Cli care_cli = {
	.version  = VERSION,
	.name     = "care",
	.subtitle = "Comprehensive Archiver for Reproducible Execution",
	.synopsis = "care [option] ... command",
	.colophon = "Visit http://reproducible.io for help, bug reports, suggestions, patches, ...\n\
Copyright (C) 2014 STMicroelectronics, licensed under GPL v2 or later.",
	.logo = "\
  _____ ____ _____ ____\n\
 /   __/ __ |  __ \\  __|\n\
/   /_/     |     /  __|\n\
\\_____|__|__|__|__\\____|",

	.pre_initialize_bindings  = pre_initialize_bindings,
	.post_initialize_bindings = post_initialize_bindings,

	.options = {
	{ .class = "Options",
	  .arguments = {
		{ .name = "-o", .separator = ' ', .value = "path" },
		{ .name = "--output", .separator = '=', .value = "path" },
		{ .name = NULL, .separator = '\0', .value = NULL } },
	  .handler = handle_option_o,
	  .description = "Archive in *path*, its suffix specifies the format.",
	  .detail = NULL,
	},
	{ .class = "Options",
	  .arguments = {
		{ .name = "-c", .separator = ' ', .value = "path" },
		{ .name = "--concealed-path", .separator = '=', .value = "path" },
		{ .name = NULL, .separator = '\0', .value = NULL } },
	  .handler = handle_option_c,
	  .description = "Make *path* content appear empty during the original execution.",
	  .detail = NULL,
	},
	{ .class = "Options",
	  .arguments = {
		{ .name = "-r", .separator = ' ', .value = "path" },
		{ .name = "--revealed-path", .separator = '=', .value = "path" },
		{ .name = NULL, .separator = '\0', .value = NULL } },
	  .handler = handle_option_r,
	  .description = "Make *path* content accessible when nested in a concealed path.",
	  .detail = NULL,
	},
	{ .class = "Options",
	  .arguments = {
		{ .name = "-p", .separator = ' ', .value = "path" },
		{ .name = "--volatile-path", .separator = '=', .value = "path" },
		{ .name = NULL, .separator = '\0', .value = NULL } },
	  .handler = handle_option_p,
	  .description = "Don't archive *path* content, reuse actual *path* instead.",
	  .detail = NULL,
	},
	{ .class = "Options",
	  .arguments = {
		{ .name = "-e", .separator = ' ', .value = "name" },
		{ .name = "--volatile-env", .separator = '=', .value = "name" },
		{ .name = NULL, .separator = '\0', .value = NULL } },
	  .handler = handle_option_e,
	  .description = "Don't archive *name* env. variable, reuse actual value instead.",
	  .detail = NULL,
	},
	{ .class = "Options",
	  .arguments = {
		{ .name = "-m", .separator = ' ', .value = "value" },
		{ .name = "--max-archivable-size", .separator = '=', .value = "value" },
		{ .name = NULL, .separator = '\0', .value = NULL } },
	  .handler = handle_option_m,
	  .description = "Set the maximum size of archivable files to *value* megabytes.",
	  .detail = NULL,
	},
	{ .class = "Options",
	  .arguments = {
		{ .name = "-d", .separator = '\0', .value = NULL },
		{ .name = "--ignore-default-config", .separator = '\0', .value = NULL },
		{ .name = NULL, .separator = '\0', .value = NULL } },
	  .handler = handle_option_d,
	  .description = "Don't use the default options.",
	  .detail = NULL,
	},
	{ .class = "Options",
	  .arguments = {
		{ .name = "-v", .separator = ' ', .value = "value" },
		{ .name = "--verbose", .separator = '=', .value = "value" },
		{ .name = NULL, .separator = '\0', .value = NULL } },
	  .handler = handle_option_v,
	  .description = "Set the level of debug information to *value*.",
	  .detail = NULL,
	},
	{ .class = "Options",
	  .arguments = {
		{ .name = "-V", .separator = '\0', .value = NULL },
		{ .name = "--version", .separator = '\0', .value = NULL },
		{ .name = "--about", .separator = '\0', .value = NULL },
		{ .name = NULL, .separator = '\0', .value = NULL } },
	  .handler = handle_option_V,
	  .description = "Print version, copyright, license and contact, then exit.",
	  .detail = NULL,
	},
	{ .class = "Options",
	  .arguments = {
		{ .name = "-x", .separator = ' ', .value = "file" },
		{ .name = "--extract", .separator = '=', .value = "file" },
		{ .name = NULL, .separator = '\0', .value = NULL } },
	  .handler = handle_option_x,
	  .description = "Extract content of the archive *file*, then exit.",
	  .detail = NULL,
	},
	{ .class = "Options",
	  .arguments = {
		{ .name = "-h", .separator = '\0', .value = NULL },
		{ .name = "--help", .separator = '\0', .value = NULL },
		{ .name = "--usage", .separator = '\0', .value = NULL },
		{ .name = NULL, .separator = '\0', .value = NULL } },
	  .handler = handle_option_h,
	  .description = "Print the user manual, then exit.",
	  .detail = NULL,
	},
	{0},
	},
};

#endif /* CARE_CLI_H */
