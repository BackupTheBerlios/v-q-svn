<?xml version="1.0" encoding="iso-8859-2"?>
<xsl:stylesheet version="1.0"
	         xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:variable name="redir_msgs" select="document(concat('file://credirmod.',$LOCALE,'.xml'))/msgs"/>

<xsl:template match="vqwww/mod/conf/mod/redir">
	<xsl:value-of select="$redir_msgs/menu/mod/m[@n='redir']"/>
</xsl:template>

<xsl:template match="vqwww/mod/conf/mod/desc/redir">
	<xsl:value-of select="$redir_msgs/menu/mod/desc/m[@n='redir']"/>
</xsl:template>

<xsl:template name="input">
	<!-- input form -->
	<table align="center">
		<xsl:for-each select="item">
			<tr>
				<td>
					<xsl:value-of select="$redir_msgs/mod/redir/m[@n='email']"/>
				</td>
				<td>
					<input>
						<xsl:attribute name="name">
							<xsl:text>item</xsl:text><xsl:value-of select="position()"/>
						</xsl:attribute>
	
						<xsl:attribute name="value">
							<xsl:value-of select="@val"/>
						</xsl:attribute>
					</input>

					<input type="hidden">
						<xsl:attribute name="name">
							<xsl:text>itemid</xsl:text><xsl:value-of select="position()"/>
						</xsl:attribute>

						<xsl:attribute name="value">
							<xsl:value-of select="@id"/>
						</xsl:attribute>
					</input>
				</td>
			</tr>
		</xsl:for-each>
	</table>
</xsl:template>

<xsl:template match="vqwww/mod/redir">
	<form method="post">
		<table width="100%">
			<xsl:choose>
				<xsl:when test="getcant">
					<tr class="err">
						<td>
							<xsl:value-of select="$redir_msgs/mod/redir/m[@n='getcant']"/>
						</td>
					</tr>
				</xsl:when>

				<xsl:otherwise>
					<tr class="desc">
						<td>
							<xsl:value-of select="$redir_msgs/mod/redir/m[@n='desc']"/>
						</td>
					</tr>

					<!-- error messages -->
					<xsl:for-each select="inv/rm">
						<tr class="err">
							<td>
								<xsl:value-of select="$redir_msgs/mod/redir/m[@n='inv/rm']"/>
								<xsl:value-of select="text()"/>
							</td>
						</tr>
					</xsl:for-each>

					<xsl:for-each select="inv/add">
						<tr class="err">
							<td>
								<xsl:value-of select="$redir_msgs/mod/redir/m[@n='inv/add']"/>
								<xsl:value-of select="text()"/>
							</td>
						</tr>
					</xsl:for-each>

					<xsl:for-each select="inv/rep">
						<tr class="err">
							<td colspan="2">
								<xsl:value-of select="$redir_msgs/mod/redir/m[@n='inv/rep']"/>
								<xsl:value-of select="text()"/>
							</td>
						</tr>
					</xsl:for-each>
				
					<tr>
						<td>
							<xsl:call-template name="input"/>
						</td>
					</tr>

					<tr>
						<td align="center">
							<input type="submit" class="button">
								<xsl:attribute name="value">
									<xsl:value-of select="$redir_msgs/mod/redir/m[@n='submit']"/>
								</xsl:attribute>
							</input>
						</td>
					</tr>

					<input type="hidden" name="id" value="redir"/>
					<input type="hidden" name="conf" value="1"/>
				</xsl:otherwise>
			</xsl:choose>
		</table>
	</form>
</xsl:template>

</xsl:stylesheet>
