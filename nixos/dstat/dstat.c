#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <limits.h>
#include <fcntl.h>
#include <time.h>
#include <sys/resource.h>

#define D_V "0.3"
#define D_YR "2015-2016"
#define D_BUF 64

#define AC_GLYPH "▲"
#define BATT_GLYPH "▼"
#define BATT_FULL_GLYPH "❤"
#define UNMUTE_GLYPH "🔈"

#define COLOR_NORM "\x01"
#define COLOR_WARN "\x03"
#define COLOR_CRIT "\x04"

#define d_warn(s) (warn(s), s)


static const char *d_bar(unsigned char p) {
    const char *s[] = { "▁", "▂", "▃", "▄", "▅", "▆", "▇", "█", "█" };

    return s[((8 * p) / 100)];
}

static char *d_fmt(char *s, size_t sz, const char *fmt, ...) {
    va_list va;
    int r;

    va_start(va, fmt), r = vsnprintf(s, sz, fmt, va), va_end(va);
    if (r < 0 || (size_t)r >= sz)
        return d_warn("vsnprintf failed");
    return s;
}

#define D_KILO (1024)
#define D_MEGA (1024 * 1024)
#define D_GIGA (1024 * 1024 * 1024)

static char *d_bytes(char *s, size_t sz, double b) {
    if (b > D_GIGA)
        return d_fmt(s, sz, "%0.2fGB/s", b / D_GIGA);
    else if (b > D_MEGA)
        return d_fmt(s, sz, "%0.2fMB/s", b / D_MEGA);
    else if (b > D_KILO)
        return d_fmt(s, sz, "%0.2fKB/s", b / D_KILO);
    else
        return d_fmt(s, sz, "%0.0fB/s", b);
}

/* static char *d_in(uint64_t b) { */
/*     static char s[D_BUF]; */
/*     static uint64_t in; */
/*     uint64_t diff = b - in; */

/*     return in = b, d_bytes(s, sizeof(s), diff); */
/* } */

/* static char *d_out(uint64_t b) { */
/*     static char s[D_BUF]; */
/*     static uint64_t out; */
/*     uint64_t diff = b - out; */

/*     return out = b, d_bytes(s, sizeof(s), diff); */
/* } */

/* static char *d_net(const char *ifn) { */
/*     static char s[D_BUF]; */
/*     struct ifaddrs *ifas, *ifa; */
/*     struct if_data *ifd = NULL; */

/*     if (getifaddrs(&ifas) == -1) */
/*         return d_warn("getifaddrs failed"); */
/*     for (ifa = ifas; ifa && !ifd; ifa = ifa->ifa_next) */
/*         if (!strcmp(ifa->ifa_name, ifn) && ifa->ifa_data) */
/*             ifd = (struct if_data *)ifa->ifa_data; */
/*     return !ifd ? "interface failed" : */
/*         d_fmt(s, sizeof(s), "↑ %s ↓ %s", */
/*             d_out(ifd->ifi_obytes), d_in(ifd->ifi_ibytes)); */
/* } */

/* static char *d_perf(void) { */
/*     static char s[D_BUF]; */
/*     int mib[2] = { CTL_HW, HW_CPUSPEED }, frq, p; */
/*     size_t sz = sizeof(frq); */

/*     if (sysctl(mib, 2, &frq, &sz, NULL, 0) == -1) */
/*         return d_warn("sysctl failed"); */
/*     mib[1] = HW_SETPERF, sz = sizeof(p); */
/*     if (sysctl(mib, 2, &p, &sz, NULL, 0) == -1) */
/*         return d_warn("sysctl failed"); */
/*     return d_fmt(s, sizeof(s), "%0.1fGHz [%d%%]", frq / (double)1000, p); */
/* } */

/* static char *d_cpu(void) { */
/*     static char s[D_BUF]; */
/*     static long cpu[CPUSTATES]; */
/*     int mib[2] = { CTL_KERN, KERN_CPTIME }, p; */
/*     long c[CPUSTATES]; */
/*     size_t sz = sizeof(c); */

/*     if (sysctl(mib, 2, &c, &sz, NULL, 0) == -1) */
/*         return d_warn("sysctl failed"); */
/*     p = (c[CP_USER] - cpu[CP_USER] + c[CP_SYS] - cpu[CP_SYS] + */
/*          c[CP_NICE] - cpu[CP_NICE]) / (double) */
/*         (c[CP_USER] - cpu[CP_USER] + c[CP_SYS] - cpu[CP_SYS] + */
/*          c[CP_NICE] - cpu[CP_NICE] + c[CP_IDLE] - cpu[CP_IDLE]) * 100; */
/*     memmove(cpu, c, sizeof(cpu)); */
/*     return d_fmt(s, sizeof(s), "CPU %d%% %s %s", p, d_bar(p), d_perf()); */
/* } */

/* static char *d_bat(int fd) { */
/*     static char s[D_BUF]; */
/*     struct apm_power_info api; */

/*     if (ioctl(fd, APM_IOC_GETPOWER, &api) == -1) */
/*         return d_warn("ioctl failed"); */
/*     return (api.ac_state == APM_AC_ON) ? */
/*         d_fmt(s, sizeof(s), "⚡ %d%% %s [A/C]", */
/*             api.battery_life, d_bar(api.battery_life)) : */
/*         d_fmt(s, sizeof(s), "⚡ %d%% %s [%u:%02u]", */
/*             api.battery_life, d_bar(api.battery_life), */
/*             api.minutes_left / 60, api.minutes_left % 60); */
/* } */

/* static char *d_temp(void) { */
/*     static char s[D_BUF]; */
/*     struct sensordev sd; */
/*     struct sensor sn; */
/*     size_t sd_sz = sizeof(sd), sn_sz = sizeof(sn); */
/*     int mib[5] = { CTL_HW, HW_SENSORS, 0, SENSOR_TEMP, 0 }; */
/*     int64_t t = -1; */

/*     for (mib[2] = 0; ; mib[2]++) { */
/*         if (sysctl(mib, 3, &sd, &sd_sz, NULL, 0) == -1) { */
/*             if (errno == ENXIO) */
/*                 continue; */
/*             else if (errno == ENOENT) */
/*                 break; */
/*             return d_warn("sysctl failed"); */
/*         } */
/*         for (mib[4] = 0; mib[4] < sd.maxnumt[SENSOR_TEMP]; mib[4]++) { */
/*             if (sysctl(mib, 5, &sn, &sn_sz, NULL, 0) == -1) { */
/*                 if (errno == ENXIO) */
/*                     continue; */
/*                 else if (errno == ENOENT) */
/*                     break; */
/*                 return d_warn("sysctl failed"); */
/*             } */
/*             if (sn_sz && !(sn.flags & SENSOR_FINVALID)) */
/*                 t = sn.value > t ? sn.value : t; */
/*         } */
/*     } */
/*     return t == -1 ? "temperature failed" : */
/*         d_fmt(s, sizeof(s), "T %.1f°C", (t - 273150000) / 1000000.0); */
/* } */

/* static char *d_vol(int fd) { */
/*     static char s[D_BUF]; */
/*     static int cls = -1; */
/*     struct mixer_devinfo mdi; */
/*     struct mixer_ctrl mc; */
/*     int v = -1, p; */

/*     for (mdi.index = 0; cls == -1; mdi.index++) { */
/*         if (ioctl(fd, AUDIO_MIXER_DEVINFO, &mdi) == -1) */
/*             return d_warn("ioctl failed"); */
/*         if (mdi.type == AUDIO_MIXER_CLASS && */
/*             !strcmp(mdi.label.name, AudioCoutputs)) */
/*                 cls = mdi.index; */
/*     } */
/*     for (mdi.index = 0; v == -1; mdi.index++) { */
/*         if (ioctl(fd, AUDIO_MIXER_DEVINFO, &mdi) == -1) */
/*             return d_warn("ioctl failed"); */
/*         if (mdi.type == AUDIO_MIXER_VALUE && mdi.prev == AUDIO_MIXER_LAST && */
/*             mdi.mixer_class == cls && !strcmp(mdi.label.name, AudioNmaster)) { */
/*             mc.dev = mdi.index; */
/*             if (ioctl(fd, AUDIO_MIXER_READ, &mc) == -1) */
/*                 return d_warn("ioctl failed"); */
/*             v = mc.un.value.num_channels == 1 ? */
/*                 mc.un.value.level[AUDIO_MIXER_LEVEL_MONO] : */
/*                 (mc.un.value.level[AUDIO_MIXER_LEVEL_LEFT] > */
/*                  mc.un.value.level[AUDIO_MIXER_LEVEL_RIGHT] ? */
/*                  mc.un.value.level[AUDIO_MIXER_LEVEL_LEFT] : */
/*                  mc.un.value.level[AUDIO_MIXER_LEVEL_RIGHT]); */
/*         } /\* todo: handle mute *\/ */
/*     } */
/*     return v == -1 ? "volume failed" : */
/*         (p = (v * 100) / 255, d_fmt(s, sizeof(s), "♫ %d%% %s", p, d_bar(p))); */
/* } */

char *
smprintf(char *fmt, ...)
{
  va_list fmtargs;
  char *buf = NULL;

  va_start(fmtargs, fmt);
  if (vasprintf(&buf, fmt, fmtargs) == -1){
    fprintf(stderr, "malloc vasprintf\n");
    exit(1);
  }
  va_end(fmtargs);

  return buf;
}

char *
readfile(char *base, char *file) {
  char *path, line[513];
  FILE *fd;

  path = smprintf("%s/%s", base, file);
  fd = fopen(path, "r");
  if (fd == NULL)
    return NULL;
  free(path);

  memset(line, 0, sizeof(line));
  if (fgets(line, sizeof(line) - 1, fd) == NULL)
    return NULL;
  fclose(fd);

  return smprintf("%s", line);
}

static char *d_time(void) {
    static char s[D_BUF];
    struct tm *tm;
    time_t now = time(0);
    
    tm = localtime(&now);
    strftime(s, sizeof(s), "%a%d%b, %H:%M", tm);
    return s;
}

static char *d_bat() {
  static char s[D_BUF];
  static char *base = "/sys/class/power_supply/BAT0";
  char *r;

  int fullcap, currcap;
  int status = 0;

  r = readfile(base, "status");
  if (r == NULL)
    return smprintf("");
  if (strcmp(r, "Charging\n") == 0)
    status = 1;
  else if (strcmp(r, "Full\n") == 0)
    status = 2;

  r = readfile(base, "charge_full");
  if (r == NULL)
    return smprintf("");
  sscanf(r, "%d", &fullcap);
  free(r);

  r = readfile(base, "charge_now");
  if (r == NULL)
    return smprintf("");
  sscanf(r, "%d", &currcap);
  free(r);

  
  float bat = ((float)currcap / (float)fullcap) * 100;
  bat = bat > 100 ? 100 : bat;
  
  char *color = COLOR_NORM;
  if (status == 0) {
    if (bat < 5) color = COLOR_CRIT;
    else if (bat < 10) color = COLOR_WARN;
  }

  return smprintf("%s%s%0.f%%%s",
		  color,
		  status == 2 ? BATT_FULL_GLYPH : (status == 1 ? AC_GLYPH : BATT_GLYPH),
		  bat,
		  COLOR_NORM);
}

static char *d_vol(int fd) {
  static char s[D_BUF];
  return s;
}

static char *d_wifi(const char *ifn) {
  static char s[D_BUF];
  return s;
}

static void d_run(const char *ifn) {
    Display *d;
    int a = -1, m = -1;
    char s[LINE_MAX];

    if (!(d = XOpenDisplay(NULL)))
        err(1, "XOpenDisplay failed");
    /* if ((a = open("/dev/apm", O_RDONLY)) == -1 || */
    /*     (m = open("/dev/mixer", O_RDONLY)) == -1) */
    /*     err(1, "open failed"); */
    for (;;) {
        /* XStoreName(d, DefaultRootWindow(d), */
        /*     d_fmt(s, sizeof(s), "%s %s %s %s", */
        /*         d_wifi(ifn), d_bat(a), d_vol(m), d_time())); */
        XStoreName(d, DefaultRootWindow(d),
		   d_fmt(s, sizeof(s), "%s %s| %s",
			 UNMUTE_GLYPH,
			 d_bat(), d_time()));
        printf("%s\n", s);
        XSync(d, False), sleep(1);
    }
    close(m), close(a);
    XCloseDisplay(d);
}

int main(int argc, char *argv[]) {
    if (argc == 2 && !strcmp(argv[1], "version")) {
        puts("dstat "D_V" (c) "D_YR" Joerg Jung");
        return 0;
    }
    if (argc != 2)
        errx(1, "Usage: %s <if>\n%13s %s version", argv[0], "", argv[0]);
    if (setpriority(PRIO_PROCESS, getpid(), 10))
        err(1, "setpriority failed");
    d_run(argv[1]);
    return 0;
}
