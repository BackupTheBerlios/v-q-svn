<?xml version="1.0" encoding="iso-8859-2"?>
<xsl:stylesheet version="1.0"
	         xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:variable name="autoresp_msgs" select="document(concat('file://cautorespmod.',$LOCALE,'.xml'))/msgs"/>

<xsl:template match="vqwww/mod/conf/mod/autoresp">
	<xsl:value-of select="$autoresp_msgs/menu/mod/m[@n='autoresp']"/>
</xsl:template>

<xsl:template match="vqwww/mod/conf/mod/desc/autoresp">
	<xsl:value-of select="$autoresp_msgs/menu/mod/desc/m[@n='autoresp']"/>
</xsl:template>

<xsl:template match="vqwww/mod/autoresp">
	<form method="post">
		<table width="100%">
			<xsl:choose>
				<xsl:when test="getcant">
					<tr class="err">
						<td>
							<xsl:value-of select="$autoresp_msgs/mod/autoresp/m[@n='getcant']"/>
						</td>
					</tr>
				</xsl:when>
				<xsl:otherwise>
					<!-- error messages -->
					<xsl:if test="chgcant">
						<tr class="err">
							<td colspan="2">
								<xsl:value-of select="$autoresp_msgs/mod/autoresp/m[@n='chgcant']"/>
							</td>
						</tr>
					</xsl:if>

					<tr class="desc">
						<td>
							<xsl:value-of select="$autoresp_msgs/mod/autoresp/m[@n='desc']"/>
						</td>
					</tr>

					<!-- input form -->
					<tr>
						<td>
							<xsl:value-of select="$autoresp_msgs/mod/autoresp/m[@n='label']"/>
						</td>
					</tr>
					<tr>
						<td align="center">
							<textarea name="autoresp" cols="60" rows="20">
								<xsl:value-of select="autoresp"/>
							</textarea>
						</td>
					</tr>

					<tr>
						<td align="center">
							<input class="button" type="submit">
								<xsl:attribute name="value">
									<xsl:value-of select="$autoresp_msgs/mod/autoresp/m[@n='submit']"/>
								</xsl:attribute>
							</input>
						</td>
					</tr>

					<input type="hidden" name="id" value="autoresp"/>
					<input type="hidden" name="conf" value="1"/>
				</xsl:otherwise>
			</xsl:choose>
		</table>
	</form>
</xsl:template>

</xsl:stylesheet>
