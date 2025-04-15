import { defineComponent, PropType } from "vue";
import { RouterLink } from "vue-router";
import { RouteRecordRaw } from "vue-router";

import router from "../../router";

export default defineComponent({
	name: "Navbar",
	components: {
		RouterLink,
	},
	props: {
		isMobile: {
			type: Boolean,
			required: true,
		},
		isMenuOpen: {
			type: Boolean,
			required: true,
		},
		menuItems: {
			type: Array as PropType<Array<{ title: string; icon: string }>>,
			required: true,
		},
	},
	computed: {
		routes() {
			return router
				.getRoutes()
				.map((route: RouteRecordRaw) => ({
					name: route.name as string,
					path: route.path,
				}))
				.sort();
		},
	},
	methods: {
		beforeEnter(el: HTMLElement) {
			el.style.maxHeight = "0";
		},
		enter(el: HTMLElement) {
			el.style.transition = "max-height 0.5s ease-in-out";
			el.style.maxHeight = el.scrollHeight + "px";
		},
		leave(el: HTMLElement) {
			el.style.maxHeight = el.scrollHeight + "px";
			setTimeout(() => {
				el.style.maxHeight = "0";
			});
		},
	},
});
