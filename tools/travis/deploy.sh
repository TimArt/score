#!/bin/bash -eux
if [[ "$TRAVIS_TAG" = "" ]];
then
  TRAVIS_TAG=devel
fi

export TAG=$(echo $TRAVIS_TAG | tr -d v)
case "$CONF" in
  osx-package)
    export HOMEBREW_NO_AUTO_UPDATE=1
    export SRC_PATH=/Users/travis/build/ossia/score
    brew install graphicsmagick imagemagick npm
    npm install --global create-dmg

    cd $SRC_PATH/bundle/
    mkdir $SRC_PATH/deploy

    # Codesign
    security unlock-keychain -p travis build.keychain
    
    codesign \
      --entitlements $SRC_PATH/src/vstpuppet/entitlements.plist \
      --force \
      --timestamp \
      --options=runtime \
      --sign "ossia.io" \
      score.app/Contents/MacOS/ossia-score-vstpuppet.app

    codesign \
      --entitlements $SRC_PATH/src/app/entitlements.plist \
      --force \
      --timestamp \
      --options=runtime \
      --sign "ossia.io" \
      score.app

    # Create a .dmg
    cp $SRC_PATH/LICENSE.txt license.txt
    security unlock-keychain -p travis build.keychain
    create-dmg 'score.app'
    ls

    # Notarize the .dmg
    security unlock-keychain -p travis build.keychain
    xcrun altool --notarize-app \
                 -t osx \
                 -f *.dmg \
                 --primary-bundle-id "io.ossia.score" \
                 -u jeanmichael.celerier@gmail.com \
                 -p "@env:MAC_ALTOOL_PASSWORD" > altool.log

    REQ_UUID=$(cat altool.log | grep Request | awk ' { print $3 } ')

    # Wait until the notarization process completes to staple the result to the dmg file
    sleep 30
    for seconds in 30 30 30 30 30; do
      RES=$(xcrun altool --notarization-info $REQ_UUID -u  jeanmichael.celerier@gmail.com -p "@env:MAC_ALTOOL_PASSWORD" --output-format xml)
      if [[ $(echo "$RES" | grep Approved) ]]; then
        xcrun stapler staple *.dmg
        echo "Stapling successful !"
        break
      elif [[ $(echo "$RES" | grep -i progress) ]]; then
        sleep $seconds
      else
        echo "Stapling failed ! Displaying log..."
        echo "$RES"
      fi
    done

    mv *.dmg "$SRC_PATH/deploy/ossia score-$TAG-macOS.dmg"
    mv "mac-sdk.zip" "$SRC_PATH/deploy/"
  ;;
  linux-package-appimage)
    cd /home/travis/build/ossia/score/
    mkdir deploy
    mv "Score.AppImage" "deploy/ossia score-$TAG-linux-amd64.AppImage"
    mv "linux-sdk.zip" "deploy/"
  ;;
  tarball)
    cd /home/travis/build/ossia/score/
    mkdir deploy
    mv "ossia-score.tar.xz" "deploy/ossia score-$TAG-src.tar.xz"
    mv "ossia-score.tar.xz.asc" "deploy/ossia score-$TAG-src.tar.xz.asc"
  ;;
esac
