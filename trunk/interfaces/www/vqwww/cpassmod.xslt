<?xml version="1.0" encoding="iso-8859-2"?>
<xsl:stylesheet version="1.0"
	         xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:variable name="pass_msgs" select="document(concat('file://cpassmod.',$LOCALE,'.xml'))/msgs"/>

<xsl:template match="vqwww/mod/conf/mod/pass">
	<xsl:value-of select="$pass_msgs/menu/mod/m[@n='pass']"/>
</xsl:template>

<xsl:template match="vqwww/mod/conf/mod/desc/pass">
	<xsl:value-of select="$pass_msgs/menu/mod/desc/m[@n='pass']"/>
</xsl:template>

<xsl:template match="vqwww/mod/pass">
	<form method="post">
		<table align="center">
			<xsl:choose>
				<xsl:when test="inv/done">
					<tr class="err">
						<td colspan="2">
							<xsl:value-of select="$pass_msgs/mod/pass/m[@n='inv/done']"/>
						</td>
					</tr>
				</xsl:when>
				<xsl:otherwise>
					<xsl:if test="inv/cpass/@empty">
						<tr class="err">
							<td colspan="2">
								<xsl:value-of select="$pass_msgs/mod/pass/m[@n='inv/cpass/@empty']"/>
							</td>
						</tr>
					</xsl:if>
					<xsl:if test="inv/npass/@empty">
						<tr class="err">
							<td colspan="2">
								<xsl:value-of select="$pass_msgs/mod/pass/m[@n='inv/npass/@empty']"/>
							</td>
						</tr>
					</xsl:if>
					<xsl:if test="inv/npass/@dirty">
						<tr class="err">
							<td colspan="2">
								<xsl:value-of select="$pass_msgs/mod/pass/m[@n='inv/npass/@dirty']"/>
							</td>
						</tr>
					</xsl:if>
					<xsl:if test="inv/npass/@tooshort">
						<tr class="err">
							<td colspan="2">
								<xsl:value-of select="$pass_msgs/mod/pass/m[@n='inv/npass/@tooshort']"/>
							</td>
						</tr>
					</xsl:if>
					<xsl:if test="inv/npass/@toolong">
						<tr class="err">
							<td colspan="2">
								<xsl:value-of select="$pass_msgs/mod/pass/m[@n='inv/npass/@toolong']"/>
							</td>
						</tr>
					</xsl:if>
					<xsl:if test="inv/dontmatch">
						<tr class="err">
							<td colspan="2">
								<xsl:value-of select="$pass_msgs/mod/pass/m[@n='inv/dontmatch']"/>
							</td>
						</tr>
					</xsl:if>
					<xsl:if test="inv/oldbad">
						<tr class="err">
							<td colspan="2">
								<xsl:value-of select="$pass_msgs/mod/pass/m[@n='inv/oldbad']"/>
							</td>
						</tr>
					</xsl:if>
					<xsl:if test="inv/chgcant">
						<tr class="err">
							<td colspan="2">
								<xsl:value-of select="$pass_msgs/mod/pass/m[@n='inv/chgcant']"/>
							</td>
						</tr>
					</xsl:if>
					<xsl:if test="inv/getcant">
						<tr class="err">
							<td colspan="2">
								<xsl:value-of select="$pass_msgs/mod/pass/m[@n='inv/getcant']"/>
							</td>
						</tr>
					</xsl:if>

					<tr>
						<td>
							<xsl:value-of select="$pass_msgs/mod/pass/m[@n='cpass']"/>
						</td>
						<td>
							<input type="password" name="cpass"/>
						</td>
					</tr>
					
					<tr>
						<td>
							<xsl:value-of select="$pass_msgs/mod/pass/m[@n='npass']"/>
						</td>
						<td>
							<input type="password" name="npass"/>
						</td>
					</tr>
					
					<tr>
						<td>
							<xsl:value-of select="$pass_msgs/mod/pass/m[@n='rpass']"/>
						</td>
						<td>
							<input type="password" name="rpass"/>
						</td>
					</tr>

					<tr>
						<td colspan="2" align="center">
							<input type="submit" value="Zmień" class="button">
								<xsl:attribute name="value">
									<xsl:value-of select="$pass_msgs/mod/pass/m[@n='submit']"/>
								</xsl:attribute>
							</input>
						</td>
					</tr>

					<input type="hidden" name="id" value="pass"/>
					<input type="hidden" name="conf" value="1"/>
				</xsl:otherwise>
			</xsl:choose>
		</table>
	</form>
</xsl:template>

</xsl:stylesheet>
