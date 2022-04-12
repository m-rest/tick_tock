# HOW TO NOTARIZE FOR APPLE

**1. download apple developer certificate**
  - developer.apple.com -> Certificates/Identifiers/Profiles -> (+)
  - type: “Developer ID Application”

**2. Update Apple Worldwide Developer Relations Certificate if necessary and certificate is untrusted:**

> “Apple has updated Developer Relations Intermediate Certificate. Just download 
> the certificate from [^1] and install it. If the it doesn't works have a look on [^2]“ *from https://developer.apple.com/forums/thread/662300*

[^1]: https://www.apple.com/certificateauthority/AppleWWDRCAG3.cer
[^2]: https://developer.apple.com/support/expiration/

**3. create app specific password**
  - login into appleid.apple.com -> signin & security -> app specific passwords
  - create new app-specific password for atool

**4. compile plugin as hardened runtime (ProJucer feature)**

**6. sign, overwriting previous signature. add timestamp**
  - codesign -s "Developer ID Application: Erfindungsbuero GmbH " Multiclock.component --timestamp --force

**7. compress as zip**

**8. upload to notarize**
 > xcrun altool --notarize-app --primary-bundle-id "com.erm.multiclock" --username "DEVELOPER APPLE ID HERE" --password "APP SPECIFIC PASSWORD HERE" --asc-provider "ASC PROVIDER HERE" --file Multiclock.component.zip
  
  - replace —password with own app specific password

**9. check status**
  > xcrun altool --username "apple@e-rm.de" --password "twjy-xong-myem-rgcj" --notarization-info 208e4754-2770-4cac-9ce2-8ba94add223e

**10. staple notarization to file**
  > xcrun stapler staple Multiclock.component

**11. verify**
  > codesign --test-requirement="=notarized" --verify --verbose Multiclock.component

**12. compress**

**13. ship**
