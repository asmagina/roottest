#pragma read \
  source="Event::fNewData" \
  version="[-2]" \
  target="Event::fAnotherNewData" \
  targetType="float" \
  embed="true" \
  code="{ newObj->fAnotherNewData = old->fNewData; }"
