/**
 * MetaTagDefinition - Interface for defining meta tags in a Vue application.
 *
 * This interface is used to describe the shape of meta tag objects, which are used
 * to dynamically set meta tags in the document head, such as for SEO purposes.
 * It supports standard meta tag attributes like 'name', 'content', and 'charset',
 * and allows for additional custom attributes as needed.
 *
 * @Author: Simon Marcel Linden
 * @Version: 1.0.0
 * @since: 1.0.0
 */
interface MetaTagDefinition {
	name?: string;
	content?: string;
	charset?: string;
	[key: string]: string | undefined;
}

export default MetaTagDefinition;
