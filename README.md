# PSP Everest 2, frostegater and Joel16

# Description:
PSP Everest is an application similar to PSPident that displays more information about your PSP Console, using the native PSP UI (VLF). This is a mod of the original PSP Everest 2 by frostegater. This application has been built with the latest toolchain with major changes to display various new info and fix a few bugs.

# Changes PSP Everest 2 Rev 6 + (forked version)
- Built with the latest toolchain, with upstream GCC and newlib patches.
- New Console ID Information page:
  - Display PSID.
  - Display company code.
  - Display factory code and info.
  - Display product code and info.
  - Display product sub code and info.
- Battery Information page now displays the following information:
  - Display battery elec value
  - Display battery total elec value
- Display polestar version in Hardware Information page.
- Identify TMU-00X, TA-079v1-v5, TA-095v3, TA-095v4 and TA-097 motherboards.
- Display QA flag in Hardware Information page.
- Fix static title bar colour when changing background colours with L/R.
- Make use of VLFlib's central menu instead of hard coding positions for the main menu.
- Major code clean-up, eliminated all warnings, prevent overflows, limiting buffer lenghts etc.

# Credits:
frostegater, jas0nuk, Yoti, raing3, Total_Noob, some1, m0skit0, TyRaNiD, ErikPshat, vit9696, rewind83 and psdevwiki contributors.

# Tested:
- PSP-E1000 (TA-096)
- PSP-N1000 (TA-091)
- PSP-1000 (TA-081)
- PSP-2000 (TA-085v1)
- PSP-2000 (TA-088v1/v2)
- PSP-3000 (TA-090v2)

# License:
- GPLv3 (for more information use gpl-v3.txt in 'src' directory)
