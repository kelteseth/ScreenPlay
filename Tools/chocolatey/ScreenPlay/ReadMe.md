# Choco

Build locally:
```
choco pack
```

Install the generated nupkg:
```
choco install screenplay.0.15.0-RC2.nupkg -dv -s .  
```

Set api key from [https://community.chocolatey.org/account](https://community.chocolatey.org/account):
```
choco apikey --key AAAAAAAA-AAAA-AAAA-AAAA-AAAAAAAA --source https://push.chocolatey.org/
```

Psuh:
```
choco push screenplay.0.15.0-RC2.nupkg --source https://push.chocolatey.org/
```