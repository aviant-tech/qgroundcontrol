# QGroundControl Ground Control Station

[![Releases](https://img.shields.io/github/release/mavlink/QGroundControl.svg)](https://github.com/mavlink/QGroundControl/releases)
[![Travis Build Status](https://travis-ci.org/mavlink/qgroundcontrol.svg?branch=master)](https://travis-ci.org/mavlink/qgroundcontrol)
[![Appveyor Build Status](https://ci.appveyor.com/api/projects/status/crxcm4qayejuvh6c/branch/master?svg=true)](https://ci.appveyor.com/project/mavlink/qgroundcontrol)

[![Gitter](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/mavlink/qgroundcontrol?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

_QGroundControl_ (QGC) is an intuitive and powerful ground control station (GCS) for UAVs.

The primary goal of QGC is ease of use for both first time and professional users.
It provides full flight control and mission planning for any MAVLink enabled drone, and vehicle setup for both PX4 and ArduPilot powered UAVs. Instructions for _using QGroundControl_ are provided in the [User Manual](https://docs.qgroundcontrol.com/en/) (you may not need them because the UI is very intuitive!)

All the code is open-source, so you can contribute and evolve it as you want.
The [Developer Guide](https://dev.qgroundcontrol.com/en/) explains how to [build](https://dev.qgroundcontrol.com/en/getting_started/) and extend QGC.

Key Links:

- [Website](http://qgroundcontrol.com) (qgroundcontrol.com)
- [User Manual](https://docs.qgroundcontrol.com/en/)
- [Developer Guide](https://dev.qgroundcontrol.com/en/)
- [Discussion/Support](https://docs.qgroundcontrol.com/en/Support/Support.html)
- [Contributing](https://dev.qgroundcontrol.com/en/contribute/)
- [License](https://github.com/mavlink/qgroundcontrol/blob/master/COPYING.md)

### Running unit tests

[Guide](https://aviant.atlassian.net/wiki/spaces/TECHNICAL/pages/994803739/Running+unit+tests)

### Update the latest QGC version in AWS
You need to have an AWS account with the necessary permissions for ECRPublic and configure the [AWS CLI](https://docs.aws.amazon.com/cli/latest/userguide/cli-chap-configure.html).
1. Log in to AWS ecr public: `aws ecr-public get-login-password --region us-east-1 | docker login --username AWS --password-stdin public.ecr.aws`
2. Pull the desired release (image tag) you want to set to latest: `docker pull public.ecr.aws/m7l5v7s4/qgroundcontrol:<release version>`
3. Tag the release as "latest": `docker tag public.ecr.aws/m7l5v7s4/qgroundcontrol:<release version> public.ecr.aws/m7l5v7s4/qgroundcontrol:latest`
4. Push the "latest" tag: `docker push public.ecr.aws/m7l5v7s4/qgroundcontrol:latest`
5. Log out of AWS ecr public: `docker logout public.ecr.aws`
