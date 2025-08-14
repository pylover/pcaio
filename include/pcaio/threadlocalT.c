static TL_TYPE() *_tl;


void
TL_NAME(_set) (TL_TYPE() *val) {
    _tl = val;
}


TL_TYPE() *
TL_NAME(_get) () {
    return _tl;
}
