## Setup

Setup SSH config for easier future access:

```sh
mkdir -vp ~/.ssh
rainfall_address='192.168.56.106' # CHANGE ME
printf 'Host rainfall\n\tHostname %s\n\tPort 4242\n' "${rainfall_address}" | tee -a ~/.ssh/config
```
