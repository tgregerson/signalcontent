import re

#Example: epim_1_0_timing.txt:  Data Path Delay:        9.506ns  (logic 6.362ns (66.926%)  route 3.144ns (33.074%))"
timing_regex = re.compile('.*epim_([0-9]+)_([0-9]+)_.*Data Path Delay:[ ]*([0-9.]+)ns.*logic ([0-9.]+)ns.*route ([0-9.]+)ns.*')

#Example: | Slice LUTs*             |  519 |     0 |    134600 |  0.38 |
utilization_regex = re.compile('.*epim_([0-9]+)_([0-9]+)_.*Slice LUTs[*|\s]+([0-9]+)[|\s]+[0-9]+[|\s]+[0-9]+[|\s]+[0-9.]+.*')

with open("/localhome/gregerso/temp/all_data_path_timing.txt") as input_file:
  for entry in input_file:
    m = timing_regex.match(entry)
    if m:
      print m.group(1) + ',' + m.group(2) + ',' + m.group(3) + ',' + m.group(4) + ',' + m.group(5)
    else:
      print "Failed to match: ", entry

'''
with open("/localhome/gregerso/temp/all_lut_utilization.txt") as input_file:
  for entry in input_file:
    m = utilization_regex.match(entry)
    if m:
      print m.group(1) + "," + m.group(2) + "," + m.group(3)
'''
