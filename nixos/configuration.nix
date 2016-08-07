# Edit this configuration file to define what should be installed on
# your system.  Help is available in the configuration.nix(5) man page
# and in the NixOS manual (accessible by running ‘nixos-help’).

{ config, pkgs, ... }:

let
     dwm-HEAD = pkgs.callPackage ./dwm/dwm.nix {};
     dstat-HEAD = pkgs.callPackage ./dstat/dstat.nix {};
     st-HEAD = pkgs.callPackage ./st/st.nix {};
in
{
  imports =
    [ # Include the results of the hardware scan.
      ./hardware-configuration.nix
    ];


  # Use the gummiboot efi boot loader.
  boot.loader.gummiboot.enable = true;
  boot.loader.efi.canTouchEfiVariables = true;

  boot.initrd.kernelModules = ["fbcon"];
  boot.initrd.luks.devices = [
    { name="luksroot"; device="/dev/sda2"; preLVM=true; }
  ];

  boot.cleanTmpDir = true;
  boot.extraModprobeConfig = ''
    options snd_hda_intel index=0 model=intel-mac-auto id=PCH
    options snd_hda_intel index=1 model=intel-mac-auto id=HDMI
    options snd_hda_intel model=mbp101
    options hid_apple fnmode=2
  '';

  networking.hostName = "mir"; # Define your hostname.
  networking.extraHosts = "127.0.0.1 i";
  networking.wireless.enable = true;  # Enables wireless support via wpa_supplicant.
  networking.firewall.enable = true;
  networking.enableIPv6 = false;

  # Select internationalisation properties.
  i18n = {
    consoleFont = "Lat2-Terminus16";
    consoleKeyMap = "us";
    defaultLocale = "en_US.UTF-8";
  };

  # Set your time zone.
  time.timeZone = "Asia/Kolkata";

  nixpkgs.config.allowUnfree = true;
  nixpkgs.config.chromium = {
    enablePepperFlash = true;
    enableWideVine = true;
  };
  

  environment.systemPackages = with pkgs; [
    dwm-HEAD
    dmenu
    dunst
    dstat-HEAD
    st-HEAD

    acpi
    wget
    tabbed
    slock
    xorg.xbacklight
    xbindkeys
    unzip
    zip
    scrot
    htop
    nix-repl
    jwhois
    which
    tree

    tmux
    tabbed
    git
    emacs

    chromium
    vlc
    ffmpeg-full
    transmission_gtk
    youtube-dl
  ];

  security.setuidPrograms = ["slock"];
  services.ntp.enable = true;

  services.xserver = {
    enable = true;
    autorun = true;
    
    displayManager = {
      sessionCommands = ''
        xrandr --output eDP1 --scale 0.6x0.6
        xbindkeys
	dstat wlp3s0 &
      '';
      slim = {
        enable = true;
        defaultUser = "nanda";
        theme = pkgs.fetchurl {
          url = "file:///etc/nixos/custom-nixos-slim-theme.tar.gz";
          sha256 = "bade636789f6d755ba80d98715b73b216b55233c68ed88f53fadf5054cfca0a0";
        };
      };
    };

    windowManager = {
      default = "none";
      session = [{
        name = "dwm";
        start = "/run/current-system/sw/bin/dwm";
      }];
    };

    desktopManager = {
      default = "none";
      xterm.enable = false;
    };

    xkbOptions = "ctrl:nocaps";
    synaptics = {
      enable = true;
      twoFingerScroll = true;
      palmDetect = true;
      tapButtons = true;
      accelFactor = "0.0010";
      buttonsMap = [1 3 2];
    }; 
  };

  # Define a user account. Don't forget to set a password with ‘passwd’.
  users.extraUsers.nanda = {
    isNormalUser = true;
    uid = 1000;
    extraGroups = [ "wheel" ];
    createHome = true;
  };

  programs.bash.enableCompletion = true;

  # The NixOS release to be compatible with for stateful data such as databases.
  system.stateVersion = "15.09";

  fonts = {
    enableCoreFonts = false;
    enableFontDir = true;
    extraFonts = with pkgs; [
      terminus_font
      ubuntu_font_family
      dejavu_fonts
      vistafonts
      inconsolata
      terminus_font
    ];
  };

  nix = {
    extraOptions = ''
      gc-keep-outputs = true
      gc-keep-derivations = true
      env-keep-derivations = true
    '';
  };
}
