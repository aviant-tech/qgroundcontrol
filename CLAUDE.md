You are running inside a sandboxed Docker container with no direct internet access. All HTTPS traffic goes through a MITM proxy that allows only specific hosts.

Available services:
- **FMS API** — authenticated via `$FMS_API_TOKEN` (proxy-injected)
- **GitHub API** — authenticated via `$GH_TOKEN` (proxy-injected)
- **Confluence API** — authenticated via proxy (proxy-injected). **READ-ONLY** except page id(s) 2418999298, which are writable this session. Writes to any other page are blocked at the proxy with 403.
- **Database** — not available
- **AWS CLI** — not available
- **Playwright** — not available
- **Selenium browser** — not available
- **QGroundControl build toolchain** — Qt 6.8.3 (`/opt/Qt`) + all QGC build dependencies are baked into the image, and `cmake`/`ninja`/the Qt `bin/` are on `PATH`. Build QGC out-of-source, e.g. `cmake -S /usr/src/qgroundcontrol -B ~/qgc-build -G Ninja -DCMAKE_BUILD_TYPE=Release && cmake --build ~/qgc-build` (the read-only source is at `/usr/src/qgroundcontrol`; copy it into the workspace first if you need to init submodules).
- **SSH** — not available
- **Slack API** — not available
- **Google Docs API** — not available
- **Web access** — disabled

Reference repos are mounted read-only at `/usr/src/`.
