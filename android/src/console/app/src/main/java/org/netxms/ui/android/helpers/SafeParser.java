package org.netxms.ui.android.helpers;

/**
 * Helper for safe parsing of values (from string)
 * 
 * @author Marco Incalcaterra (marco.incalcaterra@thinksoft.it)
 *
 */

public class SafeParser
{
	/**
	 * Parse int without throwing exception
	 * @param text text to parse
	 * @param defVal default value to be used in case of parse error
	 * @return parsed value
	 */
	static public int parseInt(String text, int defVal)
	{
		try
		{
			return Integer.parseInt(text);
		}
		catch (NumberFormatException e)
		{
			return (int)parseFloat(text, defVal);	// Try to parse as float and truncate decimal part
		}
	}

	/**
	 * Parse long without throwing exception
	 * @param text text to parse
	 * @param defVal default value to be used in case of parse error
	 * @return parsed value
	 */
	static public long parseLong(String text, long defVal)
	{
		try
		{
			return Long.parseLong(text);
		}
		catch (NumberFormatException e)
		{
			return (long)parseFloat(text, defVal);	// Try to parse as float and truncate decimal part
		}
	}

	/**
	 * Parse float without throwing exception
	 * @param text text to parse
	 * @param defVal default value to be used in case of parse error
	 * @return parsed value
	 */
	static public float parseFloat(String text, float defVal)
	{
		try
		{
			return Float.parseFloat(text);
		}
		catch (NumberFormatException e)
		{
		}
		return defVal;
	}

	/**
	 * Parse double without throwing exception
	 * @param text text to parse
	 * @param defVal default value to be used in case of parse error
	 * @return parsed value
	 */
	static public double parseDouble(String text, double defVal)
	{
		try
		{
			return Double.parseDouble(text);
		}
		catch (NumberFormatException e)
		{
		}
		return defVal;
	}
}
