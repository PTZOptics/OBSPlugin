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

#### Running GitHub Action
1. Navigate to repository which should have GitHub Action
2. Go to `Actions` > `Build ptz-obs-plugin for Windows`
3. Click `Run workflow` > `Run workflow`
4. Refresh page
5. Click first `Build ptz-obs-plugin for Windows` item in the list of workflow runs
6. Click `build-windows`
7. Monitor build script output
8. Once build is complete, navigate to `Actions` > `Build ptz-obs-plugin for Windows` > first `Build ptz-obs-plugin for Windows` item in the list of workflow runs
9. Download `PtzObsPluginInstaller-win-x64` from artifacts section
