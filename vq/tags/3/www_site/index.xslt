<?xml version="1.0" encoding="iso-8859-2"?>
<xsl:stylesheet version="1.0"
	         xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="html" indent="yes"/>

<xsl:template match="page">
<table>
	<xsl:for-each select="msg">
	<tr><td valign="top">
		<xsl:apply-templates/>
	</td></tr>
	</xsl:for-each>
</table>
</xsl:template>

</xsl:stylesheet>

