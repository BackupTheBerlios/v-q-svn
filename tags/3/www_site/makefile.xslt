<?xml version="1.0" encoding="iso-8859-2"?>
<xsl:stylesheet version="1.0"
	         xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="text" indent="no"/>

<xsl:template match="page">
	<xsl:text>all: index.html </xsl:text>
		<xsl:for-each select="files">
			<xsl:for-each select="file">
				<xsl:value-of select="@name"/><xsl:text>.html </xsl:text>
			</xsl:for-each>
			<xsl:for-each select="log">
				<xsl:value-of select="@name"/><xsl:text>.html </xsl:text>
			</xsl:for-each>
		</xsl:for-each>
	<xsl:text>&#10;&#10;</xsl:text>
	
	<xsl:for-each select="files">
		<xsl:for-each select="file">
			<!-- rule to copy them from .. -->
			<xsl:value-of select="@name"/><xsl:text>: ../</xsl:text>
			<xsl:value-of select="@name"/><xsl:text>&#10;</xsl:text>
			<xsl:text>&#9;cp -f ../</xsl:text><xsl:value-of select="@name"/><xsl:text> .&#10;</xsl:text>
			<xsl:text>&#9;chmod 644 </xsl:text><xsl:value-of select="@name"/><xsl:text>&#10;&#10;</xsl:text>
			
			<!-- rule to create html .. -->
			<xsl:value-of select="@name"/><xsl:text>.html: </xsl:text>
			<xsl:value-of select="@name"/><xsl:text> head.html tail.html pre.inc pre_end.inc&#10;</xsl:text>
			<xsl:text>&#9;cat head.html pre.inc </xsl:text><xsl:value-of select="@name"/><xsl:text> pre_end.inc tail.html \&#10;</xsl:text>
			<xsl:text>&#9;&gt; </xsl:text><xsl:value-of select="@name"/><xsl:text>.html.tmp &amp;&amp; mv -f </xsl:text>
				<xsl:value-of select="@name"/><xsl:text>.html.tmp </xsl:text>
				<xsl:value-of select="@name"/><xsl:text>.html&#10;</xsl:text>
			<xsl:text>&#9;chmod 644 </xsl:text><xsl:value-of select="@name"/><xsl:text>.html&#10;&#10;</xsl:text>

		</xsl:for-each>

		<xsl:for-each select="log">
			<!-- rule to copy them from .. -->
			<xsl:value-of select="@name"/><xsl:text>:&#10;</xsl:text>
			<xsl:text>&#9;cm log &gt;</xsl:text><xsl:value-of select="@name"/><xsl:text>&#10;</xsl:text>
			<xsl:text>&#9;chmod 644 </xsl:text><xsl:value-of select="@name"/><xsl:text>&#10;&#10;</xsl:text>
			
			<!-- rule to create html .. -->
			<xsl:value-of select="@name"/><xsl:text>.html: pre.inc pre_end.inc </xsl:text>
			<xsl:value-of select="@name"/><xsl:text> head.html tail.html &#10;</xsl:text>
			<xsl:text>&#9;cat head.html pre.inc </xsl:text><xsl:value-of select="@name"/><xsl:text> pre_end.inc tail.html \&#10;</xsl:text>
			<xsl:text>&#9;&gt; </xsl:text><xsl:value-of select="@name"/><xsl:text>.html.tmp &amp;&amp; mv -f </xsl:text>
				<xsl:value-of select="@name"/><xsl:text>.html.tmp </xsl:text>
				<xsl:value-of select="@name"/><xsl:text>.html&#10;</xsl:text>
			<xsl:text>&#9;chmod 644 </xsl:text><xsl:value-of select="@name"/><xsl:text>.html&#10;&#10;</xsl:text>

		</xsl:for-each>

	</xsl:for-each>
</xsl:template>

</xsl:stylesheet>

