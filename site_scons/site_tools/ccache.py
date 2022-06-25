def exists():
    return True

def generate(env):
    env['CCACHE'] = env.WhereIs("ccache")
    env['CC'] = '$CCACHE %s' % env['CC']
    # Tricky place: linking is done with CXX
    # Using ccache breaks it
    env['LINK'] = env['CXX']
    env['CXX'] = '$CCACHE %s' % env['CXX']
