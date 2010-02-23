grep -e "r -t " smart_antenna_sample.tr > temp1.tr
grep -e "tcp -Il 960" temp1.tr > temp2.tr
grep -e "-Nl AGT " temp2.tr > smart_antenna_sample.received

rm -rf temp1.tr temp2.tr

