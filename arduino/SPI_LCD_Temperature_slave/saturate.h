#pragma once

template<typename T> 
void saturate(const T minValue, T& value, const T maxValue)
{
  value = max(minValue, min(value, maxValue));
}
