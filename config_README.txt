After installing Mailman, set the following variables inside *prefix*/mailman/Mailman/mm_cfg.py:
    
    LIST_EXPIRATION_URL = ssg-test.nws.oregonstate.edu
    ENABLE_CAS = True
    CAS_SERVER_URL = login.oregonstate.edu 
    USE_CAS_DEV = True #use login.oregonstate.edu/cas-dev if True
