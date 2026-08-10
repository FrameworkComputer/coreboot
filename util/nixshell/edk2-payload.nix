with import <nixpkgs> {};

(import ./devshell-x86.nix).overrideAttrs (old: {
	name = "coreboot-edk2-payload";

	nativeBuildInputs = old.nativeBuildInputs or [] ++ [
		imagemagick
		libarchive
		openssl
		python3
		util-linux
	];

	shellHook = old.shellHook or "" + ''
		unset NIX_HARDENING_ENABLE
	'';
})
