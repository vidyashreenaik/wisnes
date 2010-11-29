grep -e "r -t " wired-cum-wireless-sim-hwmp.tr > temp1.tr
grep -e "tcp -Il 1040" temp1.tr >temp2.tr
grep -e "tcp -Il 1040" temp1.tr >temp2.tr
grep -e "-Nl AGT " temp2.tr > datarecv.tr
rm -f temp1.tr
rm -f temp2.tr