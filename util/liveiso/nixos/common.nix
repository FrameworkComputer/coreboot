# SPDX-License-Identifier: GPL-2.0-only

{ config, options, pkgs, ... }:

{
	imports = [
		<nixpkgs/nixos/modules/installer/cd-dvd/channel.nix>
		<nixpkgs/nixos/modules/installer/cd-dvd/iso-image.nix>
	];

	# Build coreboot-utils from this local coreboot checkout instead of the
	# revision nixpkgs pins, so local edits under util/ land in the ISO.
	# nixpkgs stays pinned -- .override re-evaluates the pinned coreboot-utils
	# package with only its `fetchgit` swapped for our local tree. Because every
	# tool (cbmem, cbfstool, ...) and the buildEnv share that one fetchgit, this
	# single override covers them all.
	nixpkgs.overlays = [
		(final: prev:
			let
				corebootRoot = ../../..; # this file lives in util/liveiso/nixos
				rootStr = toString corebootRoot;
				# Big dirs (relative to the coreboot root) the util builds don't
				# need; dropped so we don't copy gigabytes into the Nix store.
				# Match by RELATIVE PATH, not basename: src/ has its own
				# vendorcode/intel/fsp that cbfstool needs, so we can't blanket
				# drop every dir named "fsp".
				drop = [
					"build" "payloads" "util/crossgcc"
					"3rdparty/fsp" "3rdparty/blobs" "3rdparty/amd_blobs"
					"3rdparty/arm-trusted-firmware" "3rdparty/stm"
					"3rdparty/intel-microcode" "3rdparty/qc_blobs"
				];
				corebootSrc = builtins.path {
					name = "coreboot-local-src";
					path = corebootRoot;
					filter = path: type:
						let rel = prev.lib.removePrefix (rootStr + "/") (toString path); in
						baseNameOf path != ".git"
						&& !(prev.lib.any (d: rel == d || prev.lib.hasPrefix (d + "/") rel) drop);
				};
			in {
				coreboot-utils = prev.coreboot-utils.override { fetchgit = _: corebootSrc; };
			})
	];

	system.stateVersion = "26.05";

	isoImage = {
		makeEfiBootable = true;
		makeUsbBootable = true;
	};
	image.fileName = "${config.image.baseName}-${config.system.nixos.label}-${pkgs.stdenv.hostPlatform.system}.iso";

	environment = {
		variables = {
			EDITOR = "nvim";
			VISUAL = "nvim";
			# Tell the Nix evaluator to garbage collect more aggressively.
			# This is desirable in memory-constrained environments that don't
			# (yet) have swap set up.
			GC_INITIAL_HEAP_SIZE = "1M";
		};
		shellAliases.vim = "nvim";
	};

	boot = {
		kernelParams = [
			"console=ttyS0,115200"
			"console=tty0"
			"iomem=relaxed"
			"spi_intel.writeable=1"
		];
		# pkgs.linuxPackages == lts
		# pkgs.linuxPackages_latest == stable
		kernelPackages = pkgs.linuxPackages;
		extraModulePackages = with config.boot.kernelPackages; [
			acpi_call
			chipsec
		];
		# Make programs more likely to work in low memory
		# environments. The kernel's overcommit heustistics bite us
		# fairly often, preventing processes from forking even if
		# there is plenty of free memory.
		kernel.sysctl."vm.overcommit_memory" = "1";
		loader.grub.memtest86.enable = true;
		postBootCommands = ''
			mkdir -p /mnt
		'';
	};

	console.packages = options.console.packages.default ++ [ pkgs.terminus_font ];

	nixpkgs.config.allowUnfree = true;
	hardware = {
		cpu.intel.updateMicrocode = true;
		cpu.amd.updateMicrocode = true;
		enableAllFirmware = true;
		bluetooth = {
			enable = true;
			powerOnBoot = false;
		};
	};

	services = {
		fwupd.enable = true;
		udev.packages = with pkgs; [
			rfkill_udev
		];
		openssh = {
			enable = true;
			settings.PermitRootLogin = "yes";
		};
	};

	networking = {
		hostName = "devsystem";
		networkmanager.enable = true;
	};

	security.sudo.wheelNeedsPassword = false;

	users = {
		groups.user = {};
		users = {
			root.initialHashedPassword = "";
			user = {
				isNormalUser = true;
				group = "user";
				extraGroups = [ "users" "wheel" "networkmanager" "uucp" "flashrom" ];
				initialHashedPassword = "";
			};
		};
	};

	programs.flashrom.enable = true;

	environment.systemPackages = with pkgs; [
		acpica-tools
		btrfs-progs
		bzip2
		cabextract
		ccrypt
		chipsec
		coreboot-utils
		cryptsetup
		curl
		ddrescue
		devmem2
		dmidecode
		dosfstools
		e2fsprogs
		efibootmgr
		efivar
		exfat
		f2fs-tools
		flashprog
		fuse
		fuse3
		fwts
		gptfdisk
		gitFull
		tig
		gzip
		hdparm
		hexdump
		htop
		i2c-tools
		innoextract
		intel-gpu-tools
		inxi
		iotools
		jfsutils
		jq
		lm_sensors
		mdadm
		minicom
		mkpasswd
		ms-sys
		msr-tools
		mtdutils
		neovim
		nixos-install-tools
		ntfsprogs
		nvme-cli
		openssl
		p7zip
		pacman
		parted
		pcimem
		pciutils
		phoronix-test-suite
		powertop
		psmisc
		python3
		rsync
		screen
		sdparm
		smartmontools
		socat
		sshfs-fuse
		testdisk
		tmate
		tmux
		tpm-tools
		tpm2-tools
		uefitool
		uefitoolPackages.old-engine
		unshield
		unzip
		upterm
		usbutils
		wget
		zfs
		zip
		zstd
	];
}
