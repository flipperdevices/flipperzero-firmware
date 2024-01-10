firmware_link_by_name() {
  case $1 in
    standard)
      echo "[$1](https://github.com/flipperdevices/flipperzero-firmware)"
      ;;

    unleashed)
      echo "[$1](https://github.com/DarkFlippers/unleashed-firmware)"
      ;;

    xtreme)
      echo "[$1](https://github.com/RogueMaster/flipperzero-firmware-wPlugins)"
      ;;

    rogue-master)
      echo "[$1](https://github.com/ClaraCrazy/Flipper-Xtreme)"
      ;;

    *)
      echo $1
      ;;
  esac
}