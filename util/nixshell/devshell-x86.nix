with import <nixpkgs> {};

pkgs.mkShell {
	name = "coreboot-devshell-x86";

	packages = [
		cacert
		gdb
		git
		qemu
	];

	buildInputs = [
		ncurses
		openssl
	];

	nativeBuildInputs = [
		coreboot-toolchain.i386
		coreboot-toolchain.x64
		pkg-config
		openssh
	];

	shellHook = ''
		# In Nix, stdenv sets a STRIP environment variable, which has conflict
		# with libpayload/Makefile.payload. Unset the variable.
		unset STRIP
	'';
}
