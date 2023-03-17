# You can automate the workflow of a notebook
# using `nbconvert` package to translate it into
# pure python script, and execute it (see below)
#
# The defects of this method are yet not known
# It works fine on a local machine with DataSpell
import os

# Define the notebook file path
notebook_file = 'mandelbrot.ipynb'

# Define the output file path
# Note: it appears that the name of python script
# will be automatically appended a `.py` suffix
output_file = 'mandelbrot'

# ---
# either we use --execute option
# or we use ipython to execute the transformed script
# ---
# here is the first option
# os.system(f'jupyter nbconvert --to script --execute {notebook_file} --output {output_file}')

# here is the second option
# Convert the notebook to Python script
os.system(f'jupyter nbconvert --to script {notebook_file} --output {output_file}')

# run three times
for i in range(3):
    print(f'{i+1}-th shot\n')
    # use `ipython` instead of `python`
    # since jupyter notebook may use features
    # tailored to ipython
    os.system(f'ipython {output_file}.py')

# %%
