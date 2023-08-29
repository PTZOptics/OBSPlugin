### Setting up GitHub Actions

#### Upload workflow files
Copy and push contents of `ptzoptics-obs\gh_actions\root` into root of the repository which should have GitHub Action.

#### Setup Github access token
1. Create token
 - Login to `github.com` in web-browser using account which has access to `ptzoptics-obs` repo. 
 - Go to profile > Settings
 - Go to Developer Settings > Personal access tokens > Tokens (classic)
 - Click `Generate new token` > `Generate new token (classic)`
 - Check checkboxes `repo` (main checkbox) and `org` > `read`
 - Click `Generate`
 - Copy token to clipboard
2. Create Github Actions secret
 - Navigate to repository which should have GitHub Action
 - Go to `Settings` > `Secrets and variables` > `Actions`
 - Click `New repository secret`
 - Fill `Name` field with `GH_ACCESS_TOKEN` and `Secret` with token from clipboard
 - Click `Add secret`

#### Copy certificates for macOS signing 
1. Export certificates
 - Open `KeyChain Access` app
 - Go to `login` keychain, `Certificates` tab
 - Right click certificate for code-signing, choose export, save as `CODESIGN_CERTIFICATE.p12`.
 - Right click certificate for installer-signing, choose export, save as `SIGN_INSTALLER_CERTIFICATE.p12`.
2. Create Github Actions secrets
 - Navigate to repository which should have GitHub Action
 - Go to `Settings` > `Secrets and variables` > `Actions`
 - Click `New repository secret`
 - Run `base64 -i CODESIGN_CERTIFICATE.p12 | pbcopy` in terminal, this will update clipboard.
 - Fill `Name` field with `CODESIGN_CERTIFICATE_BASE64` and `Secret` with clipboard's content
 - Click `Add secret`
 - Same way add secrets
    - `CODESIGN_P12_PASSWORD`: password for `CODESIGN_CERTIFICATE.p12`
    - `CODESIGN_IDENTITY`: identity for `CODESIGN_CERTIFICATE.p12`. Identity can be checked with `security find-identity` in terminal.
    - `SIGN_INSTALLER_CERTIFICATE_BASE64`: paste from clipboard after `base64 -i SIGN_INSTALLER_CERTIFICATE.p12 | pbcopy` executed in terminal
    - `SIGN_INSTALLER_P12_PASSWORD`: password for `SIGN_INSTALLER_CERTIFICATE.p12`
    - `SIGN_INSTALLER_IDENTITY`: identity for `SIGN_INSTALLER_CERTIFICATE.p12`. Identity can be checked with `security find-identity` in terminal.
    - `TEMP_KEYCHAIN_PASSWORD`: any password. It will be used to create temporary keychain during build.

#### Running GitHub Action
1. Navigate to repository which should have GitHub Action
2. Go to `Actions` > `Build ptz-obs-plugin for Windows/macOS Intel`
3. Click `Run workflow` > `Run workflow`
4. Refresh page
5. Click first `Build ptz-obs-plugin for Windows/macOS Intel` item in the list of workflow runs
6. Click `build-windows`
7. Monitor build script output
8. Once build is complete, navigate to `Actions` > `Build ptz-obs-plugin for Windows/macOS Intel` > first `Build ptz-obs-plugin for Windows/macOS Intel` item in the list of workflow runs
9. Download `PtzObsPluginInstaller-win-x64`/`PtzOpticsOBSPlugin_x86_64` from artifacts section
