"""
    @package
    Output: CSV (comma-separated)
    Grouped By: Value, Footprint
    Sorted By: Ref
    Fields: Ref, Qty, Value, Footprint, Farnell, RS, Mouser, Mounted

    Command line:
    python "pathToFile/bom_csv_protoelectronics.py" "%I" "%O.csv"
"""
import sys
sys.path.append('/usr/share/kicad/plugins')

# Import the KiCad python helper module and the csv formatter
import kicad_netlist_reader
import kicad_utils
import csv

# A helper function to convert a UTF8/Unicode/locale string read in netlist
# for python2 or python3
def fromNetlistText( aText ):
    if sys.platform.startswith('win32'):
        try:
            return aText.encode('utf-8').decode('cp1252')
        except UnicodeDecodeError:
            return aText
    else:
        return aText

# Generate an instance of a generic netlist, and load the netlist tree from
# the command line option. If the file doesn't exist, execution will stop
net = kicad_netlist_reader.netlist(sys.argv[1])

# Open a file to write to, if the file cannot be opened output to stdout
# instead
try:
    f = kicad_utils.open_file_write(sys.argv[2], 'w')
except IOError:
    e = "Can't open output file for writing: " + sys.argv[2]
    print(__file__, ":", e, sys.stderr)
    f = sys.stdout

# Create a new csv writer object to use as the output formatter
out = csv.writer(f, delimiter=',', quotechar='\"', quoting=csv.QUOTE_ALL)

# Output a set of rows for a header providing general information
out.writerow(['Ref', 'Qty', 'Part number', 'Value', 'Footprint', 'Farnell', 'RS', 'Mouser'])

# Get all of the components in groups of matching parts + values
# (see ky_generic_netlist_reader.py)
grouped = net.groupComponents()

# Output all of the component information
for group in grouped:
    refs = ""

    # Add the reference of every component in the group and keep a reference
    # to the component so that the other data can be filled in once per group
    for component in group:
        refs += fromNetlistText( component.getRef() ) + ", "
        c = component

    # Fill in the component groups common data
    out.writerow([refs, len(group),
        fromNetlistText(c.getField("Part number")),
        fromNetlistText(c.getValue()),
        fromNetlistText(c.getFootprint()),
        fromNetlistText(c.getField("Farnell")),
        fromNetlistText(c.getField("RS")),
        fromNetlistText(c.getField("Mouser"))
        ])


