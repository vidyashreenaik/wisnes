grep -e "s -t " smart_antenna_sample.tr > temp1.tr
grep -e "tcp -Il 960" temp1.tr > temp2.tr
grep -e "-N0 AGT " temp2.tr > smart_antenna_sample.sent

rm -rf temp1.tr temp2.tr

